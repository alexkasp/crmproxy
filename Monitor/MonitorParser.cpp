#include <MonitorParser.h>
#include <iostream>

string MonitorParser::parsedata(ParamMap& data)
{
    string str = "";
    if(data["Event:"] == "Cdr")
    {
	
        str = parse_cdrevent(data["UniqueID:"],data["AccountCode:"]);
    }
    
    debugParseString(str,"Monitor");
    return str;
}

string MonitorParser::parse_cdrevent(string uniqueID,string accountCode)
{
    if(accountCode.empty())
	return "";
	
    string request = request_str;
    request+=uniqueID;
    request+="&userId=";
    request+=accountCode;
    return request;
   
}

//string for testing
// https://sipuni.com/api/testing/test

MonitorParser::MonitorParser(string request):IParser(request)
{
}