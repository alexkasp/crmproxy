#include <RecallParser.h>
#include <iostream>

int RecallParser::parsedata(ParamMap& data,string& from,string& to)
{
    if(data["Event:"] == "UserEvent")
    {
	if(data["UserEvent:"] == "Recall")
	{
    	    from = data["operator"];
    	    to = data["recallnum"];
        
    	    return 1;
	}
    }
    else
        return 0;
}

string RecallParser::parsedata(ParamMap& data)
{
    string request = "";
    if(data["Event:"] == "UserEvent")
    {
	if(data["UserEvent:"] == "Recall")
	{
    	    request = parse_recallrequest(data["operator"],data["recallnum"]);
	}
    }
    
    std::cout<<"prepare URL "<<request<<endl;
    return request;
}

/*
 prepare for URL
 http://sip.sipuni.com/IaEQvJmntW/callback.php?num=TO&callfrom=FROM
 */
string RecallParser::parse_recallrequest(string from,string to)
{
    string request = request_str;
    request+="callfrom="+from;
    request+="&num="+to;
    
    return request;
}

RecallParser::RecallParser(string requestStr):IParser(requestStr)
{

}