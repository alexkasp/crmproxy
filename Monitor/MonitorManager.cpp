#include <MonitorManager.h>
#include <boost/bind.hpp>

MonitorManager::MonitorManager(string server,string port):CRMUrlBuilder(server,port)
{
    boost::asio::ip::tcp::socket sock(io);
    sock.connect(ep);
}

int makeAction(ParamMap& data,IParser* currentParser)
{
    string request = currentParser->parsedata(data);
    if (!request.empty())
    {
        tgroup.create_thread(boost::bind(&MonitorManager::SendRequest,this,request));
        return 1;
    }
    return 0;
}

int MonitorManager::SendRequest(std::string url)
{
    boost::asio::streambuf request,responce;
    
    std::ostream request_stream(&request);
    
    request_stream << "GET " << url << " HTTP/1.0\r\n";
    request_stream << "Host: " << server << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";
    
    
    boost::asio::ip::tcp::socket sock(io);
    sock.connect(ep);
    boost::system::error_code ec;
    boost::asio::write(sock,request);
    
    boost::asio::read(sock,responce);
    
    string AnswerData = "";
    
    std::istream responce_stream(&responce);
    
    responce_stream >> AnswerData;
    std::cout<<"MONITOR MANAGER "<<AnswerData<<std::endl;
    
    sock.close();

}