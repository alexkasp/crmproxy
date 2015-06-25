#include <RegisterParser.h>

RegisterParser::RegisterParser(string str):IParser(str)
{

}

string RegisterParser::parse_peerstatus(string peer,string status,string address)
{
    string request = request_str;
    request+="&peer=";
    request+=peer;
    request+="&status=";
    request+=status;
    request+="&address=";
    request+=address;
    return request;
}
                                

string RegisterParser::parsedata(ParamMap& data)
{
    string str = "";
    if(data["Event:"] == "PeerStatus")
    {
        str = parse_peerstatus((data["Peer:"]).substr(4),data["PeerStatus:"],data["Address:"]);
    }
    else
        return str;
    
}

int RegisterParser::parsedata(ParamMap& data,string& number,string& status,string& address)
{
    if(data["Event:"] == "PeerStatus")
    {
	number = data["Peer:"].substr(4);
	status = data["PeerStatus:"];
	address = data["Address:"];
	return 1;
    }
    return 0;
}