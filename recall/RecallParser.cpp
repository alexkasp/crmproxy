#include <RecallParser.h>

int RecallParser::parsedata(ParamMap& data,string& from,string& to)
{
    if(data["Event:"] == "Recall")
    {
        from = data["operator:"];
        to = data["recallnum:"];
        
        return 1;
    }
    else
        return 0;
}

string RecallParser::parsedata(ParamMap& data)
{
    string request = "";
    if(data["Event:"] == "Recall")
    {
        request = parse_recallrequest(data["operator:"],data["recallnum"]);
    }
    
    return request;
}

string RecallParser::parse_recallrequest(string from,string to)
{
    string request = request_str;
    request+="from="+from;
    request+="&to="+to;
    
    return request;
}

RecallParser::RecallParser(string requestStr):IParser(requestStr)
{

}