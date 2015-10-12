#pragma once
#include <map>
#include <string>
#include <memory>
#include <queue>
#include <MergedCalls.h>

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
	string parse_peerstatus(string peer,string status,string address);
	string parse_agentcalled(string callid,string agent,string queueid);
	string parse_incomecall(string src,string dst,string uid,string timestamp,string callid,string srctype);
	string parse_answercall(string src,string dst,string uid,string timestamp,string callid,string calltype);
	string parse_finishcall(string src,string dst,string uid,string timestamp,string callid,string callstart,string callanswer,string status,string calltype,string callbackId,string usecrm);
	string parse_transfercall(string src,string dst,string uid,string timestamp,string callid);
	string parse_initcall(string src,string dst,string uid,string timestamp,string callid);
	string parse_outcall(string src,string dst,string uid,string timestamp,string callid);
	string parse_finishtransfer(string src, string dst, string uid, string timestamp, string callid);
	string parse_cdrevent(string callid);
	string parse_numtype(string num);
	void parse_mergecall(string newcallid,string callid);
	void parse_setcallbackId(string callid,string callbackId);
	
	string format_srcdstnum(string src,string dst);
	string  clearStorage(map<string,string>& storage,string key); 
	 CallRecords currentcalls;
	 map<string,string> event2storage;
	 map<string,int> accountcodes;
	 map<string,string> useridToCallId;
	 map<string,string> callbackIdList;
	 
	 MergedCalls mergedCalls;
public:
	Parser(const string str,LoggerModule& _lm);
	virtual ~Parser(void);
	string parsedata(ParamMap& data);
	void cleanCalls();
	const CallRecords& getCallRecords() const;
};

