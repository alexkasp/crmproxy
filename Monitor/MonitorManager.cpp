#include <MonitorManager.h>
#include <boost/bind.hpp>
#include <sstream>
#include <boost/foreach.hpp>

MonitorManager::MonitorManager(string server,string port):CRMUrlBuilder(server,port)
{
    boost::asio::ip::tcp::socket sock(io);
    sock.connect(ep);
}

int MonitorManager::makeAction(ParamMap& data,IParser* currentParser)
{
    string request = currentParser->parsedata(data);
    if (!request.empty())
    {
        tgroup.create_thread(boost::bind(&MonitorManager::SendRequestAndWaitAnswer,this,request));
        
        //SendRequestAndWaitAnswer(request);
        return 1;
    }
    return 0;
}

int MonitorManager::SendRequestAndWaitAnswer(std::string url)
{
    boost::asio::streambuf request,response;
    
    std::ostream request_stream(&request);
    
    request_stream << "GET " << url << " HTTP/1.0\r\n";
    request_stream << "Host: " << server << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";
    
    
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
    if(!ec)
    {
        std::istream response_stream(&response);
        while(std::getline(response_stream,AnswerData))
        {
            if(AnswerData=="\r")
            {
                std::getline(response_stream,AnswerData);
                break;
            }
        }
    }

    std::stringstream ss;
    ss << AnswerData;
        
    boost::property_tree::read_json(ss, pt);
    
    
    bool status = pt.get<bool>("success");
    std::cout<<"STATUS="<<status<<endl;
    
    sock.close();
}