#include <MonitorManager.h>
#include <boost/bind.hpp>
#include <sstream>
#include <boost/foreach.hpp>
#include <fstream>

using namespace std;

MonitorManager::MonitorManager(string server,string port):CRMUrlBuilder(server,port)
{
    boost::asio::ip::tcp::socket sock(io);
    sock.connect(ep);
    tgroup.create_thread(boost::bind(&MonitorManager::SendRequestCicle,this));
}

void MonitorManager::SendRequestCicle()
{
    while(1)
    {
	string msg;
	boost::this_thread::sleep( boost::posix_time::milliseconds(10000));
	if(!SendRequestWithDelay(msg))
	{
	    fstream f(zabbix_file);
	     if (f.good()) 
	     {
	         f.close();
	     }
	     else
	     {
	        f.open(zabbix_file,fstream::out);
	        f<<msg<<"\n";
	        f.close();
	     } 
	}
    }
}

int MonitorManager::SendRequestWithDelay(string& msg)
{
    bool returnStatus = true;
    
    boost::asio::ip::tcp::socket sock(io);
    sock.connect(ep);
    boost::system::error_code ec;
    
    struct timeval now;
    gettimeofday(&now,NULL);
    
    for(auto x = requestList.begin();x!=requestList.end();)
    {
    
    RequestWithTime* requestData = &(*x);
    
    //std::cout<<"now.tv_sec="<<now.tv_sec<<"-"<<requestData->timeReq<<"="<<now.tv_sec-requestData->timeReq<<endl;
    
    if((now.tv_sec-requestData->timeReq)>600)
    {
    boost::asio::streambuf request,response;
    
    std::ostream request_stream(&request);
    
    request_stream << "GET " << requestData->request << " HTTP/1.0\r\n";
    request_stream << "Host: " << server << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: Keep-Alive\r\n\r\n";
    
    
    x = requestList.erase(x);
    
    boost::asio::write(sock,request,ec);
    if(ec)
    {
        std::cout<<"ERROR MSG "<<ec.value()<<std::endl;
    }
    
    std::string AnswerData;
    boost::asio::read_until(sock,response,"\r\n\r\n",ec);
    std::istream response_stream(&response);

    if(!ec)
    {

        while(std::getline(response_stream,AnswerData))
        {
    	    std::cout<<AnswerData<<std::endl;
            if(AnswerData=="\r")
            {
        	boost::asio::read_until(sock,response,"\n",ec);
                break;
            }
        }
    }
    
    std::getline(response_stream,AnswerData);
    
    std::stringstream ss;
    ss << AnswerData;
    std::cout<<"JSON DATA "<<AnswerData<<std::endl;    
    
    
    
    try{
	boost::property_tree::read_json(ss, pt);
    }
    catch(exception& e)
    {
	std::cout<<e.what()<<std::endl;
    }
    
	bool status = pt.get<bool>("success");
	if(!status)
	{
	    msg = pt.get<string>("message");
	}
	returnStatus&=status;
    }
    else
	break;
    }
    sock.close();
    
    return returnStatus;
}

int MonitorManager::SendRequest(std::string url)
{

    struct timeval now;
    gettimeofday(&now,NULL);

    RequestWithTime newRequest;
    newRequest.timeReq = now.tv_sec;
    newRequest.request = url;
    
    requestList.push_back(newRequest);
}