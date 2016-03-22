#ifndef CDR_MANAGER
#define CDR_MANAGER 1
#include <DButils.h>
#include <LoggerModule.h>
#include <map>
#include <set>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <string>

using namespace std;


class CDRManager
{
    LoggerModule& lm;
    DButils& db;
    
    boost::asio::io_service io;
    //boost::asio::ip::tcp::socket sock;
    string baseUrl;
    string keepAliveUrl;
    string server;
    boost::thread_group tgroup;
    boost::asio::ip::tcp::endpoint ep;
    
    map<string,vector<string>> involvedNums;
    void sendCurlRequest(string url,string requestId);
    void keepAlive();
    void sendRequest(string url);
    void putCDR(map<string,string>& data);
    string map2json (const std::map<std::string, std::string>& map);
    void read_handler(boost::shared_ptr<boost::asio::streambuf> databuf,const boost::system::error_code& ec,std::size_t size);
    void addInvolvedNums(map<string,string>& data);
    public:
	CDRManager(LoggerModule& lm,string server,string port,string baseUrl,string keepAliveUrl,DButils& db);
	void processCDR(map<string,string>& data);
	~CDRManager();
};
#endif