#include <MonitorParserCRM.h>
#include <iostream>



using namespace std;

string MonitorParserCRM::parse_cdrevent(string uniqueID,string accountCode)
{
    string request = request_str;
    request+=uniqueID;
    
    return request;
    
}