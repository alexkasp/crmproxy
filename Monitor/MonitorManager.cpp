#include <MonitorManager.h>
#include <boost/bind.hpp>
#include <sstream>
#include <boost/foreach.hpp>

MonitorManager::MonitorManager(string server,string port):CRMUrlBuilder(server,port)
{
    boost::asio::ip::tcp::socket sock(io);
    sock.connect(ep);
}

int MonitorManager::SendRequest(std::string url)
{

    boost::asio::streambuf request,response;
    
    std::ostream request_stream(&request);
    
    request_stream << "GET " << url << " HTTP/1.0\r\n";
    request_stream << "Host: " << server << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: Keep-Alive\r\n\r\n";
    
    
    boost::asio::ip::tcp::socket sock(io);
    sock.connect(ep);
    boost::system::error_code ec;
    
    
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
    
    sock.close();
}