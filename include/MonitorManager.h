#include <CRMUrlBuilder.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sys/time.h>
#include <vector>

struct RequestWithTime
{
    time_t timeReq;
    string request;
};

class MonitorManager: public CRMUrlBuilder
{
    const std::string zabbix_file = "/var/lib/zabbix/asterisk.statistics_error";
    virtual int	SendRequest(std::string url);
    boost::thread_group tgroup;
    
    void SendRequestCicle();
    int SendRequestWithDelay(std::string& msg);
    
    boost::property_tree::ptree pt;
    vector<RequestWithTime> requestList;
    
protected:
    
    // virtual int makeAction(ParamMap& data,IParser* currentParser);
public:
    MonitorManager(string server,string port);
};