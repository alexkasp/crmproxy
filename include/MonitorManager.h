#include <CRMUrlBuilder.h>

class MonitorManager: public CRMUrlBuilder
{
    SendRequest(std::string url);
   
protected:
    
     virtual int makeAction(ParamMap& data,IParser* currentParser);
public:
    MonitorManager(string server,string port);
};