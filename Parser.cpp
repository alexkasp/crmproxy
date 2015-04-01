#include "Parser.h"
#include <iostream>
#include <boost/lexical_cast.hpp>

#undef CALLMANUALCONTROL

const string& CallRecord::getdst() const
{
	return dst;
}

const string& CallRecord::getuid() const
{
	return uid;
}

const string& CallRecord::gettimestamp() const
{
	return timestamp;
}

const string& CallRecord::getcallid() const
{
	return callid;
}

CallRecord::~CallRecord(void)
{
	cout<<"CallRecords delete for "<<dst<<endl;
}

CallRecord::CallRecord(string _dst,string _uid,string  _timestamp,string _callid):
	dst(_dst),uid(_uid),timestamp(_timestamp),callid(_callid)
{
	cout<<"Create Call"<<dst<<endl;
}

Parser::Parser(void):
request_str("/native/crmtest.php?userId=")
{
	//request_str = "/native/crmtest.php?userId=";
}




Parser::~Parser(void)
{
	
}

void Parser::cleanCalls()
{
	
}

string Parser::parse_numtype(string num)
{
	int num_type = (num.length()<10)+1;
	return boost::lexical_cast<std::string>(num_type);
}
string Parser::format_srcdstnum(string src,string dst)
{
	string result = "&src_num=";
	result+=src;
	result+="&src_type=";
	result+=parse_numtype(src);
	result+="&dst_num=";
	result+=dst;
	result+="&dst_type=";
	result+=parse_numtype(dst);
	return result;
}
string Parser::parse_initcall(string src,string dst,string uid,string timestamp,string callid)
{
#ifdef CALLMANUALCONTROL
	auto x = currentcalls.insert(make_pair(src,CallRecord(dst,uid,timestamp,callid)));
#endif	
	string request = request_str;
	request+=uid;
	request+="&event=1&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst);
	request+="&timestamp=";
	request+=timestamp;
	return request;
}
string Parser::parse_outcall(string src,string dst,string uid,string timestamp,string callid)
{
	string request = request_str;
	request+=uid;
	request+="&event=1&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst);
	request+="&timestamp=";
	request+=timestamp;
	return request;
}
string Parser::parse_incomecall(string src,string dst,string uid,string timestamp,string callid,string srctype)
{

	string request = request_str;
	request+=uid;
	request+="&event=1&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst);
	request+="&timestamp=";
	request+=timestamp;
	return request;
}

string Parser::parse_answercall(string src,string dst,string uid,string timestamp,string callid,string calltype)
{
	int src_type = (src.length()<10)+1;
	int dst_type = (dst.length()<10)+1;
	
	string request = request_str;
	request+=uid;
	request+="&event=3&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst);
	request+="&timestamp=";
	request+=timestamp;
	return request;
}

string Parser::parse_finishcall(string src,string dst,string uid,string timestamp,string callid,string callstart,string callanswer,string status,string calltype)
{
#ifdef CALLMANUALCONTROL
	auto x = currentcalls.begin();
	pair<decltype(x),decltype(x)> ret;
	ret = currentcalls.equal_range(src);
	for(auto x=ret.first; x!=ret.second;++x)
	{
		if((x->first==src)&&((x->second).getdst()==dst))
		{
			currentcalls.erase(x);
			break;
		}
	}
#endif	
	for (auto x = currentcalls.begin(); x != currentcalls.end(); ++x)
	{
		cout << "call records " << (*x).first << " -> " << (*x).second.getdst() << endl;
	}
	string request = request_str;
	request+=uid;
	request+="&event=2&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst);
	request+="&call_start_timestamp=";
	request+=callstart;
	request+="&call_answer_timestamp=";
	request+=callanswer;
	request+="&timestamp=";
	request+=timestamp;
	request+="&status=";
	request+=status;
	request+="&call_record_link=unknown";

	
	return request;
}
string Parser::parse_transfercall(string src,string dst,string uid,string timestamp,string callid)
{

	string request = request_str;
	request+=uid;
	request+="&event=1&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst);
	request+="&timestamp=";
	request+=timestamp;
	return request;

}

const CallRecords& Parser::getCallRecords() const
{

	return currentcalls;
}

string Parser::parsedata(ParserData& data)
{
	string str = "";
	if(data["Event:"]=="UserEvent")
	{
		
		if(data["UserEvent:"]=="incomecall")
		{
			 str = parse_incomecall(data["src"],data["dst"],data["userid"],data["time"],data["callid"],data["srctype"]);
			
		}
		if(data["UserEvent:"]=="outcall")
		{
			 str = parse_outcall(data["src"],data["dst"],data["userid"],data["time"],data["callid"]);
			
		}
		if(data["UserEvent:"]=="answercall")
		{
			 str = parse_answercall(data["src"],data["dst"],data["userid"],data["time"],data["callid"],data["calltype"]);
			
		}
		if(data["UserEvent:"]=="initcall")
		{
			 str = parse_initcall(data["src"],data["dst"],data["userid"],data["time"],data["callid"]);
			
		}
		if(data["UserEvent:"]=="transfercall")
		{
			str = parse_initcall(data["src"],data["dst"],data["userid"],data["time"],data["callid"]);
			
		}
		if(data["UserEvent:"]=="finishcall")
		{
			 str = parse_finishcall(data["src"],data["dst"],data["userid"],data["time"],data["callid"],data["callstart"],data["callanswer"],data["status"],data["calltype"]);
			
		}

		str += "&TreeId=";
		str += data["TreeId"];
	}
	else
		cout<<"wrong event "<<data["Event:"]<<endl;
	
	cout<<"Parse"<<endl<<str<<endl<<endl;
	return str;
}