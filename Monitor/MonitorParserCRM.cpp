#include <MonitorParserCRM.h>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>



using namespace std;

string MonitorParserCRM::parsedata(ParamMap& data)
{
    string str = "";
    if(data["Event:"] == "Cdr")
    {
	string channel = data["Channel:"];
	if(channel.find("Local/")==string::npos)
	{
    	    str = parse_cdrevent(data["UniqueID:"],data["AccountCode:"]);
	}
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

int MonitorParserCRM::initCrmUsers(map<string,int>& users)
{
    users.clear();
    return DBWorker.getCrmUsers(users);
}

MonitorParserCRM::MonitorParserCRM(string requeststr,string workerStr,LoggerModule& lm):MonitorParser(requeststr,lm)
{
    if(DBWorker.getAuthParams(workerStr))
    {
	DBWorker.connect();
    }
    initCrmUsers(crmUsers);
    boost::thread t(boost::bind(&MonitorParserCRM::refreshCrmUsersList,this));
}

void MonitorParserCRM::refreshCrmUsersList()
{
    while(1)
    {
	boost::this_thread::sleep( boost::posix_time::milliseconds(1800000));
	initCrmUsers(crmUsers);
    }
}
/*
void MonitorParserCRM::parse_mergecall(string newcallid,string callid)
{
    mergedCalls.addMergedCall(newcallid,callid);
}
*/
string MonitorParserCRM::parse_cdrevent(string uniqueID,string accountCode)
{
    string request;
    
    uniqueID = mergedCalls.getMergedCall(uniqueID);
    
    auto it = crmUsers.find(accountCode);
    if(it!=crmUsers.end())
    {
	if(accountCode.empty())
	    return "";
	request = request_str;
	request+=uniqueID;
	request+="&userId=";
	request+=accountCode;
    }
    return request;
    
}