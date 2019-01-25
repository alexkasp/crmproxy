#pragma once
#include <map>
#include <string>
#include <memory>
#include <queue>
#include <MergedCalls.h>
#include <set>
#include "IParser.h"
#include <boost/thread/mutex.hpp>

using namespace std;

class TransferData
{
    public:
    string callid;
    string recordname;
    string destination;
};

class CallRecord
{
	string dst;
	string src;
	string uid;
	string srctype;
	string dsttype;
	string userid;
	string callid;
	string timestamp;
	string recordfile;
	string treeid;
	string currenttreeid;
	string channel;
	string calltype;
	set<string> involvedNumbers;
	
public:
	CallRecord();
	CallRecord(string _src,string _dst,string _timestamp,string _recordfile,string userid,string uid,string srctype,string dsttype,string treeid,string channel,string calltype);
	~CallRecord();
	int addNumber(string);
	int removeNumber(string);
	int setRecordFile(string,bool);
	void setCurrentTreeId(string treeid);
	
	const string& getdst() const;
	const string& getuid() const;
	const string& getcallid() const;
	const string& gettimestamp() const;
	const string& getrecordfile() const;
	const string& getuserid() const;
	const string& getsrctype() const;
	const string& getdsttype() const;
	const string& getTreeId() const;
	const string& getCurrentTreeId() const;
	const string& getCallType() const;

	const string& getChannel() const;
};

class CallRecords
{
    boost::mutex CallLock;
    
    map<string,CallRecord> callList;

    public:
	void print();
	int size();
	CallRecords();
	void updateCall(string callid,CallRecord cr);
	void addCall(string callid,string src,string dst,string timestamp,string recordfile,string uid,string uidcode,string srctype,string dsttype,string treeid,string channel,string calltype);
	void removeCall(string callid);
	int getCall(string callid,CallRecord& cr);
	boost::mutex& getLock();
	map<string,CallRecord>& getData();
};



class Parser: public IParser
{
	string parse_qoscall(string uid,string timestamp,string callid,string rxjitter,string rxcount,string txjitter,string txcount,string rtt,string bridged_rxjitter,string bridged_rxcount,string bridged_txjitter,string bridged_txcount,string bridged_rtt);
	string parse_onlinepbx(string src,string dst,string uid,string timestamp,string callid,string uidcode,string newname);
	string parse_changetree(string uid,string callid,string newtree);
	string parse_queuecall(std::string, std::string, std::string, std::string, std::string, std::string, std::string);
	string parse_webphoneUUID(string src,string dst,string uid,string timestamp,string callid,string webphoneid);
	string parse_hangupevent(string callid);
	string parse_gatewaycall(string src,string dst,string uid);
	string parse_peerstatus(string peer,string status,string address);
	string parse_agentcalled(string callid,string agent,string queueid);
	string parse_incomecall(string src,string dst,string uid,string timestamp,string callid,string srctype,string uidcode);
	string parse_answercall(string src,string dst,string uid,string timestamp,string callid,string calltype,string usecrm,string uidcode,string channel);
	string parse_finishcall(string src,string dst,string uid,string timestamp,string callid,string callstart,string callanswer,string status,string calltype,string callbackId,string TreeID,
	string ChannelName,string serverId,string recordfile,string label,string rating,string newstatus,string crmcall,string hashtag,string usecrm,string uidcode);
	string parse_transfercall(string src,string dst,string uid,string timestamp,string callid,string uidcode);
	string parse_initcall(string src,string dst,string uid,string timestamp,string callid,string recordfile,string usecrm,
	    string uidcode,string treeid,string channel,string roistat,string roistatphone,string roistatmarket,string xcallerid,string calltype);
	string parse_outcall(string src,string dst,string uid,string timestamp,string callid,string uidcode);
	string parse_finishtransfer(string src, string dst, string uid, string timestamp, string callid,string uidcode);
	string parse_cdrevent(string callid,string destination,string duration,string billableseconds,string starttime,string endtime,string DestinationContext);
	string parse_numtype(string num,string uidcode);
	void parse_mergecall(string newcallid,string callid);
	void parse_setcallbackId(string callid,string callbackId);
	void clearCallEnviroment(string callid);
	string format_srcdstnum(string src,string dst,string uidcode,string srctype,string dsttype);
	template<typename T>void  clearStorageAlg(T& storage,string key); 
	string clearStorage(map<string,string>& storage,string key);
	void clearStorages();
	int processTransfer(string callid,string linkedid,string record,string destination);
	std::string fieldNameConverter(std::string fieldname);
//	boost::timed_mutex::scoped_lock&& getCDRLock();
	
	boost::mutex reportedCallstorageLock;
	boost::mutex transferstorageLock;
	boost::mutex event2storageLock;
	boost::timed_mutex event2CDRstorageLock;
	
	 CallRecords currentCalls;
	 map<string,TransferData> transferstorage;
	 map<string,string> event2storage;
	 map<string,string> event2CDRstorage;
	 map<string,int> accountcodes;
	 map<string,string> useridToCallId;
	 map<string,string> callbackIdList;
	 map<string,string> reportedCall;
	 
	 MergedCalls mergedCalls;
public:
	Parser(const string str,LoggerModule& _lm);
	virtual ~Parser(void);
	string parsedata(ParamMap& data);
	void cleanCalls();
	const CallRecords& getCallRecords() const;
};

