#include <CRMUrlBuilder.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class MonitorManager: public CRMUrlBuilder
{
    virtual int	SendRequest(std::string url);
    boost::thread_group tgroup;
    
    boost::property_tree::ptree pt;
    
protected:
    
    // virtual int makeAction(ParamMap& data,IParser* currentParser);
public:
    MonitorManager(string server,string port);
};