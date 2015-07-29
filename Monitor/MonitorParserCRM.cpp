#include <MonitorParserCRM.h>


using namespace std;

string MonitorParser::parse_cdrevent(string uniqueID)
{
    cout<<"MONITOR PARSER CRM"<<endl;
    string request = request_str;
    request+=uniqueID;
    return request;
    
}