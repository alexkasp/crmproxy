#pragma once
#include <map>
#include <string>



using namespace std;

class CallRecord
{
	string dst;
	string uid;
	string callid;
	string timestamp;
public:
	CallRecord(string _dst,string _uid,string  _timestamp,string _callid);
	~CallRecord();
	const string& getdst() const;
	const string& getuid() const;
	const string& getcallid() const;
	const string& gettimestamp() const;
	
};

class Parser
{
	string parse_incomecall(string src,string dst,string uid,string timestamp,string callid,string srctype);
	string parse_answercall(string src,string dst,string uid,string timestamp,string callid,string calltype);
	string parse_finishcall(string src,string dst,string uid,string timestamp,string callid,string callstart,string callanswer,string status,string calltype);
	string parse_transfercall(string src,string dst,string uid,string timestamp,string callid);
	string parse_initcall(string src,string dst,string uid,string timestamp,string callid);
	string parse_outcall(string src,string dst,string uid,string timestamp,string callid);
	string parse_numtype(string num);
	string format_srcdstnum(string src,string dst);
	const string request_str {"/native/crmtest.php?userId="};
	multimap<string,unique_ptr<CallRecord>> currentcalls;
public:
	Parser(void);
	virtual ~Parser(void);
	string parsedata(map<string,string>& data);
};

