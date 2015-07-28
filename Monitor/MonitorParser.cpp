#include <MonitorParser.h>

string MonitorParser::parsedata(PraamMap& data)
{
    string str = "";
    if(data["Event:"]=="UserEvent")
    {
        
        if(data["Event:"] == "Cdr")
        {
            str = parse_cdrevent(data["UniqueID:"]);
        }
    }
    
    return str;
}

string MonitorParser::parse_cdrevent(string uniqueID)
{
    string request = request_str;
    request+=uniqueID;
    return request;
}

//string for testing
// https://sipuni.com/api/testing/test

MonitorParser::MonitorParser(string request):IParser(request)
{

}