#ifndef ICM_SERVER
#define ICM_SERVER

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <string>
#include <LoggerModule.h>
#include <Parser.h>
#include <map>
#include <ICMStorage.h>
#include <DButils.h>

using namespace boost::asio;
using namespace std;

class ICMServer
{
    io_service service;
    ip::udp::socket socket;
    LoggerModule& lm;
    DButils& db;
    
    ICMStorage storage;
    void storeCDRData(std::map<std::string,std::string>& data);
    string convertNumber(string num);
    void getRequest();
    void prepareAccept();
    void processICM(const boost::system::error_code& error,std::size_t bytes_transferred, boost::shared_ptr<ip::udp::endpoint> sender,char* databuf);
    void solveRequest(string number,boost::shared_ptr<ip::udp::endpoint> sender);
public:
    int init(int port);
    void startProcessing();
    ICMServer(LoggerModule& _lm,DButils& db);
    int putCDREvent(map<string,string>& data);
};

#endif