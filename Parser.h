#pragma once
#include <map>
#include <string>
#include <memory>
#include <queue>

#include "IParser.h"
using namespace std;

class CallRecord
{
	string dst;
	string uid;
	string callid;
	string timestamp;

	CallRecord();
public:
	CallRecord(string _dst,string _uid,string  _timestamp,string _callid);
	~CallRecord();
	const string& getdst() const;
	const string& getuid() const;
	const string& getcallid() const;
	const string& gettimestamp() const;
	
};

using CallRecords = multimap < string, CallRecord >;


class Parser: public IParser
{
	
	string parse_incomecall(string src,string dst,string uid,string timestamp,string callid,string srctype);
	string parse_answercall(string src,string dst,string uid,string timestamp,string callid,string calltype);
	string parse_finishcall(string src,string dst,string uid,string timestamp,string callid,string callstart,string callanswer,string status,string calltype);
	string parse_transfercall(string src,string dst,string uid,string timestamp,string callid);
	string parse_initcall(string src,string dst,string uid,string timestamp,string callid);
	string parse_outcall(string src,string dst,string uid,string timestamp,string callid);
	string parse_numtype(string num);
	string format_srcdstnum(string src,string dst);
	const string request_str;
	 CallRecords currentcalls;
	
public:
	Parser(const string& str);
	virtual ~Parser(void);
	string parsedata(ParamMap& data);
	void cleanCalls();
	const CallRecords& getCallRecords() const;
};

