#include <MonitorParser.h>
#include <iostream>

string MonitorParser::parsedata(ParamMap& data)
{
    string str = "";
    if(data["Event:"] == "Cdr")
    {
	if(data["BillableSeconds:"]!="0")
    	    str = parse_cdrevent(data["UniqueID:"],data["AccountCode:"]);
    }
    else if(data["Event:"] == "UserEvent")
        {
    	    if(data["UserEvent:"] == "mergecall")
    	    {
    	        parse_mergecall(data["newcallid"],data["callid"]);
    	    }
        }
    debugParseString(str,"Monitor");
    return str;
}

void MonitorParser::parse_mergecall(string newcallid,string callid)
{
    mergedCalls.addMergedCall(newcallid,callid);
    }

string MonitorParser::parse_cdrevent(string uniqueID,string accountCode)
{
    if(accountCode.empty())
	return "";
	
    uniqueID = mergedCalls.getMergedCall(uniqueID);
    
    string request = request_str;
    request+=uniqueID;
    request+="&userId=";
    request+=accountCode;
    return request;
   
}

//string for testing
// https://sipuni.com/api/testing/test

MonitorParser::MonitorParser(string request,LoggerModule& lm):IParser(request,lm)
{
}