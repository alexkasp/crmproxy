#ifndef ICM_SERVER
#define ICM_SERVER

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <string>
#include <LoggerModule.h>
#include <Parser.h>
#include <map>

using namespace boost::asio;
using namespace std;

class ICMServer: public IParser
{
    io_service service;
    ip::udp::socket socket;
    LoggerModule& lm;
    
    
    void storeCDRData(std::map<std::string,std::string>& data);
    void getRequest();
    void prepareAccept();
    void processICM(const boost::system::error_code& error,std::size_t bytes_transferred, boost::shared_ptr<ip::udp::endpoint> sender,char* databuf);
    void solveRequest(string number,boost::shared_ptr<ip::udp::endpoint> sender);
public:
    int init(int port);
    void startProcessing();
    ICMServer(LoggerModule& _lm);
    virtual string parsedata(ParamMap& data);
    int putCDREvent(string url);
};

#endif