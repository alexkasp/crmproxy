#include <RecallParser.h>
#include <iostream>

int RecallParser::parsedata(ParamMap& data,string& from,string& to,string& announce)
{
    if(data["Event:"] == "UserEvent")
    {
	
	if(data["UserEvent:"] == "Recall")
	{
    	    from = data[fieldNameConverter("operator")];
    	    to = data[fieldNameConverter("recallnum")];
    	    announce = data[fieldNameConverter("announce")];
    	    std::cout<<"Start recall "<<from<<" ->"<<to<<" with "<<announce<<endl;
    	    return 1;
	}
    }
    return 0;
}

int RecallParser::parsedatacheckanswer(ParamMap& data,string& from,string& to,string& channel,string& callernum,string& callid,string& dialtime,string& dialargs)
{
    if(data["Event:"] == "UserEvent")
    {
	
	if(data["UserEvent:"] == "CheckAnswer")
	{
    	    from = data[fieldNameConverter("operator")];
    	    to = data[fieldNameConverter("recallnum")];
    	    channel = data[fieldNameConverter("channel")];
    	    callid = data[fieldNameConverter("callid")];
    	    dialtime = data[fieldNameConverter("dialtime")];
    	    dialargs = data[fieldNameConverter("dialargs")];
    	    callernum = data[fieldNameConverter("callernum")];
    	    
    	    std::cout<<"Start checkanswer "<<from<<" ->"<<to<<" with "<<channel<<endl;
    	    return 1;
	}
    }
    return 0;
}

string RecallParser::parsedata(ParamMap& data)
{
    string request = "";
    if(data["Event:"] == "UserEvent")
    {
	if(data["UserEvent:"] == "Recall")
	{
	    lm.makeLog(boost::log::trivial::severity_level::info,"RECALL DATA ["+data["operator"]+"] ["+data["operator:"]+"] ["+data["recallnum"]+"] ["+data["recallnum:"]+"]");
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

RecallParser::RecallParser(string requestStr,LoggerModule& lm):IParser(requestStr,lm)
{

}