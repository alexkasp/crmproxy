#include <Parser.h>
#include<ctime>
#include <chrono>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/algorithm/string.hpp>
#undef CALLMANUALCONTROL

#define ASTER_VER 13

std::string chars(
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "1234567890");

boost::random::random_device rng;
boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);

CallRecord::CallRecord()
{}

const string& CallRecord::getdst() const
{
	return dst;
}

void CallRecord::setCurrentTreeId(string newtreeid)
{
    currenttreeid = newtreeid;
}

const string& CallRecord::getCurrentTreeId() const
{
    return currenttreeid;
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

const string& CallRecord::getrecordfile() const
{
	return recordfile;
}

const string& CallRecord::getuserid() const
{
    return userid;
}

const string& CallRecord::getsrctype() const
{
    return srctype;
}

const string& CallRecord::getdsttype() const
{
    return dsttype;
}

const string& CallRecord::getTreeId() const
{
    return treeid;
}

const string& CallRecord::getChannel() const
{
    return channel;
}


CallRecord::~CallRecord(void)
{
	cout<<"CallRecords delete for "<<dst<<endl;
}

CallRecord::CallRecord(string _src,string _dst,string _timestamp,string _recordfile,string _uid,string _userid,string _srctype,string _dsttype,string _treeid,string _channel):
	dst(_dst),src(_dst),timestamp(_timestamp),recordfile(_recordfile),uid(_uid),userid(_userid),srctype(_srctype),dsttype(_dsttype),treeid(_treeid),channel(_channel)
{
	cout<<"Create Call"<<dst<<endl;
}



int CallRecord::addNumber(string num)
{
    involvedNumbers.insert(num);
    return involvedNumbers.size();
}

int CallRecord::removeNumber(string num)
{
    auto it = involvedNumbers.find(num);
    if(it!=involvedNumbers.end())
	involvedNumbers.erase(it);

    for(it=involvedNumbers.begin();it!=involvedNumbers.end();++it)
	std::cout<<"INVOLVED NUMBERS "<<(*it)<<"\n";
    return involvedNumbers.size();
}

int CallRecord::setRecordFile(string filename,bool forced=false)
{
    std::cout<<"current recordfile = "<<recordfile<<"\n";
    
    if((forced)||(!filename.empty()&&(recordfile.empty())))
    {
	std::cout<<"try set recordfile "<<filename<<" for "<<callid<<"\n";
	recordfile = filename;
	return 1;
    }
    return 0;
}

CallRecords::CallRecords()
{}



void CallRecords::print()
{
    boost::mutex::scoped_lock lock(CallLock);
    for(auto it = callList.begin();it!=callList.end();++it)
    {
	std::cout<<"Current Calls\n";
	std::cout<<"callid="<<(it->first)<<"\n";
    }

}

int CallRecords::size()
{
    return callList.size();
}

void CallRecords::addCall(string callid,string src,string dst,string timestamp,string recordfile,string uid,string uidcode,string srctype,string dsttype,string treeid,string channel)
{
    boost::mutex::scoped_lock lock(CallLock);
    
    auto it=callList.find(callid);
    
    if(it!=callList.end())
	return;
	
    CallRecord cr(src,dst,timestamp,recordfile,uidcode,uid,srctype,dsttype,treeid,channel);
    
    std::cout<<"ADD CALL with "<<callid<<" src="<<src<<" dst="<<dst<<" time="<<timestamp<<"\n";
    
    callList[callid] = cr;
}

void CallRecords::updateCall(string callid,CallRecord cr)
{
    callList[callid]=cr;
}

void CallRecords::removeCall(string callid)
{
    
    auto it = callList.find(callid);
    if(it!=callList.end())
    {
	boost::mutex::scoped_lock lock(CallLock);
	
	callList.erase(it);
    }
}

int CallRecords::getCall(string callid,CallRecord& cr)
{
    boost::mutex::scoped_lock lock(CallLock);
    map<string,CallRecord>::iterator it = callList.find(callid);
    if(it!=callList.end())
    {
	cr = it->second;
    	return 1;
    }
    
    return 0;
}

map<string,CallRecord>& CallRecords::getData()
{
    return callList;
}

Parser::Parser(const std::string str,LoggerModule& lm):
IParser(str,lm)
{
	//request_str = "/native/crmtest.php?userId=";
	boost::thread t(boost::bind(&Parser::clearStorages,this));
}


boost::mutex& CallRecords::getLock()
{
    return CallLock;
}

template <typename T>  void Parser::clearStorageAlg(T& storage,string timestamp)
{
	typename T::iterator it;
	
	for(auto it=storage.begin();it!=storage.end();)
	{
	    string timestampData = it->first;
	    if(timestampData<timestamp)
	    {
		std::cout<<"erase "<<(it->first)<<"\n";
	    	storage.erase(it++);
	    	std::cout<<"erase complete\n";
	    	
	    }
	    else
		++it;
	
	}
}

void Parser::clearStorages()
{
    using namespace boost::posix_time;
    
    while(1)
    {
	ptime now = second_clock::universal_time();
	ptime epoch = time_from_string("1970-01-01 02:00:00.000");
	time_duration const diff = now - epoch;
    
	stringstream os;
	os << diff.total_seconds();
    
	string timestamp;
	os>>timestamp;
    
	std::cout<<"TIMESTAMP="<<timestamp<<"\n";
	
	
	boost::mutex::scoped_lock lock(currentCalls.getLock());
	clearStorageAlg(currentCalls.getData(),timestamp);
	lock.unlock();
	
	std::cout<<"get lock clearStorages\n";
	boost::mutex::scoped_lock lockEvent2Storage(event2storageLock);
	clearStorageAlg(event2storage,timestamp);
	//clearStorageAlg(useridToCallId,timestamp);
	lockEvent2Storage.unlock();
	std::cout<<"release lock clearStorages\n";
	
//	boost::timed_mutex::scoped_lock& lockEvent2CDRStorage  = getCDRLock();
	
	boost::timed_mutex::scoped_lock lockEvent2CDRStorage(event2CDRstorageLock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
	if(!lockEvent2CDRStorage)
	{
	    cout<<"ERROR GET CDR LOCK in clear storage\n";
	    exit(-1);
	}    
	clearStorageAlg(event2CDRstorage,timestamp);
	//clearStorageAlg(useridToCallId,timestamp);
	lockEvent2CDRStorage.unlock();
	std::cout<<"release lock clearCDRStorages\n";
	
	
	boost::mutex::scoped_lock lockMergedCalls(mergedCalls.getLock());
	clearStorageAlg(mergedCalls.getData(),timestamp);	
	lockMergedCalls.unlock();
	
	currentCalls.print();
	mergedCalls.print();
	
	boost::this_thread::sleep( boost::posix_time::milliseconds(600000));
    }
}

Parser::~Parser(void)
{
	
}

void Parser::cleanCalls()
{
	
}

string Parser::parse_numtype(string num,string uidcode)
{
	int num_type= 0 ;
	if(!uidcode.empty())
	{
	    if((num.substr(0,6)==uidcode)||(num.substr(1,6)==uidcode))
	    return "2";
	}
		
	if(!uidcode.empty()&&(num.length()>(uidcode.length()+1)))
	    num_type = ((num.length()<10)&&((num.substr(0,uidcode.length()).compare(uidcode)==0)||((num.substr(1,uidcode.length()).compare(uidcode)==0))))+1;
	else
	    num_type = (num.length()<10)+1;
	
	return boost::lexical_cast<std::string>(num_type);
}
string Parser::format_srcdstnum(string src,string dst,string uidcode,string src_type="",string dst_type="")
{
	string result = "&src_num=";
	result+=src;
	result+="&src_type=";
	if(src_type.empty())
	    result+=parse_numtype(src,uidcode);
	else
	    result+=src_type;
	    
	result+="&dst_num=";
	result+=dst;
	result+="&dst_type=";
	if(dst_type.empty())
	    result+=parse_numtype(dst,uidcode);
	else
	    result+=dst_type;
	    
	return result;
}
string Parser::parse_initcall(string src,string dst,string uid,string timestamp,string callid,string recordfile,string usecrm,string uidcode,string treeid,string channel,string roistat,string roistatphone,string roistatmarket,string xcallerid)
{

	string request = request_str;
	request+=uid;
	request+="&event=1&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst,uidcode);
	request+="&timestamp=";
	request+=timestamp;
	request+="&roistat=";
	request+=roistat;
	request+="&roistatphone=";
	request+=roistatphone;
	request+="&roistatmarket=";
	request+=roistatmarket;
	request+="&xcallerid=";
	request+=xcallerid;
	
	useridToCallId[callid]=uid;
	
	CallRecord call;
	if(!currentCalls.getCall(callid,call))
	{
	    currentCalls.addCall(callid,src,dst,timestamp,recordfile,uid,uidcode,parse_numtype(src,uidcode),parse_numtype(dst,uidcode),treeid,channel);
	    
	}
	else
	{
	    std::cout<<"SET RECORDFILE = "<<recordfile<<"instead of "<<call.getrecordfile()<<endl;
	    call.setRecordFile(recordfile);
	    
	    currentCalls.updateCall(callid,call);
	}
	if(usecrm=="1")
	{
	    return request;
	}
	else
	{
	    request="nocrm"+request;
	}
	
	return request;
}
string Parser::parse_outcall(string src,string dst,string uid,string timestamp,string callid,string uidcode)
{
	string request = request_str;
	request+=uid;
	request+="&event=1&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst,uidcode);
	request+="&timestamp=";
	request+=timestamp;
	return request;
}
string Parser::parse_finishtransfer(string src, string dst, string uid, string timestamp, string callid,string uidcode)
{
	string request = request_str;
	request += uid;
	request += "&event=4&call_id=";
	request += callid;
	request += format_srcdstnum(src, dst,uidcode);
	request += "&timestamp=";
	request += timestamp;
	return request;
}

string Parser::parse_webphoneUUID(string src,string dst,string uid,string timestamp,string callid,string webphoneid)
{
    string request = request_str;
    request+=uid;
    request+="&event=6&src=";
    request+=src;
    request+="&dst=";
    request+=dst;
    request+="&callid=";
    request+=callid;
    request+="&webphoneid=";
    request+=webphoneid;
    request+="&timestamp=";
    request+=timestamp;
    
    std::cout<<request<<"\n";
    
    return request;

}

string Parser::parse_incomecall(string src,string dst,string uid,string timestamp,string callid,string srctype,string uidcode)
{

	string request = request_str;
	request+=uid;
	request+="&event=5&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst,uidcode);
	request+="&timestamp=";
	request+=timestamp;
	return request;
}

string Parser::parse_answercall(string src,string dst,string uid,string timestamp,string callid,string calltype,string usecrm,string uidcode,string channel)
{
	//int src_type = (src.length()<10)+1;
	//int dst_type = (dst.length()<10)+1;
	
	string request = request_str;
	request+=uid;
	request+="&event=3&call_id=";
	request+=callid;
	CallRecord call;
	if(currentCalls.getCall(callid,call))
	{
	    std::cout<<"ADD pbxdstnum for callid\n";
	    request+="&pbxdstnum=";
	    request+=call.getdst();
	    request+=format_srcdstnum(src,dst,uidcode,call.getsrctype(),call.getdsttype());
	}
	else
	    request+=format_srcdstnum(src,dst,uidcode);
	request+="&timestamp=";
	request+=timestamp;
	request+="&channel=";
	request+=channel;
	
	
	
	
	string realcallid = mergedCalls.getMergedCall(callid);
	
	if(currentCalls.getCall(realcallid,call))
	{
	    call.addNumber(dst);
	    //currentCalls.updateCall(callid,call);
	    
	}
	
	if(usecrm=="1")
	{
	    return request;
	}
	else
	{
	    request="nocrm"+request;
	}
	
	return request;
}

string Parser::parse_queuecall(string src,string dst,string uid,string timestamp,string callid,string srctype,string uidcode)
{
    return "";
}

string Parser::parse_agentcalled(string src,string dst,string callid)
{
	string request = request_str;
	CallRecord call;
	if(currentCalls.getCall(callid,call))
	{
		return parse_incomecall(src,dst,call.getuserid(),"1111",callid,call.getsrctype(),call.getuid());
	}
	
}

string Parser::parse_finishcall(string src,string dst,string uid,string timestamp,string callid,string callstart,string callanswer,string status,string calltype, 
string callbackId,string treeid, string channel,string serverId,string recordfile,string label,string rating,string newstatus,string crmcall,string hashtag,string usecrm,string uidcode)
{
	std::cout<<"START FINISHCALL\n";
	string event2store;
	
	string request = request_str;
	string src_type = "";
	string dst_type = "";
	CallRecord call;
	if(currentCalls.getCall(callid,call))
	{
	    std::cout<<"find call "<<call.getcallid()<<" with time "<<call.gettimestamp()<<"\n";
	    callstart = call.gettimestamp();
	    //if(recordfile.empty())
	    recordfile = call.getrecordfile();
	    src_type = call.getsrctype();
	    dst_type = call.getdsttype();
	}
	else
	{
	    std::cout<<"CALLRECORDS NOT FOUND FOR "<<callid<<" EXIT\n";
	    return "";
	}
	
	
	request+=uid;
	request+="&uidcode=";
	request+=uidcode;
	request+="&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst,uidcode,src_type,dst_type);
	request+="&call_start_timestamp=";
	request+=callstart;
	request+="&call_answer_timestamp=";
	request+=callanswer;
	request+="&timestamp=";
	request+=timestamp;
	request+="&status=";
	request+=status;
	request+="&call_record_link="+recordfile;
	request+="&calltype=";
	request+=calltype;
	request+="&TreeId="+treeid;
	request+="&Channel="+channel;
	request+="&serverId="+serverId;
	request+="&label="+label;
	request+="&rating="+rating;
	request+="&crmcall="+crmcall;
	request+="&hashtag="+hashtag;
	request+="&newstatus="+newstatus;
	
	event2store=request;
	event2store+="&event=2";
	
	request+="&event=4";
	
	event2store+="&callbackId=";
	event2store+=callbackId;
	
//	boost::timed_mutex::scoped_lock& lockEvent2CDRStorage  = getCDRLock();
	
	boost::timed_mutex::scoped_lock lockEvent2CDRStorage(event2CDRstorageLock,boost::get_system_time() + boost::posix_time::milliseconds(10000));	
	if(!lockEvent2CDRStorage)
	{
	    cout<<"ERROR GET CDR LOCK\n";
	    exit(-1);
	}
	
	std::cout<<"finishcall process get event2storageLock\n";
	
	auto cdrdata = event2CDRstorage.find(callid);
	
	boost::mutex::scoped_lock lockReportedCallsStorage(reportedCallstorageLock);
	auto report = reportedCall.find(callid);
	if(report!=reportedCall.end())
	    std::cout<<"REPORT for "<<callid<<" was sent early\n";
	    
	if((cdrdata!=event2CDRstorage.end())&&(!(report!=reportedCall.end())))
	{
	    
	    
	    int involvedNums = call.removeNumber(dst);
	    if(involvedNums==0)
	    {
		std::cout<<"PREPARED REQUEST=\n"<<(cdrdata->second)<<"\n";
	        event2store+=cdrdata->second;
	    
		
	        //clearCallEnviroment(callid);
	        event2CDRstorage.erase(cdrdata);
	        event2store+="&parsertask=finish";
	        std::cout<<"FINISH CALL: ADD REPORTED CALL "<<callid<<"\n";
	        reportedCall[callid]=request;
	        return event2store;
	    }
	    else
	    {
	        std::cout<<"finish call, but not last num ("<<involvedNums<<")\n";
	    }
	}
	else
	{	
	    boost::mutex::scoped_lock lockEvent2Storage(event2storageLock);
	    if(usecrm=="1")
	    {
		//boost::mutex::scoped_lock lockEvent2Storage(event2storageLock);
		event2storage[callid]=event2store;
		std::cout<<"PRINT EVENT2STORAGE\n";
		for(auto a = event2storage.begin();a!=event2storage.end();++a)
		    std::cout<<"storage "<<(a->first)<<"\n";
		return request;
	    }
	    else
	    {
		//boost::mutex::scoped_lock lockEvent2Storage(event2storageLock);
		std::cout<<"finish call no crm for "<<callid<<endl;
		
		event2store="nocrm"+event2store;
		
		event2storage[callid]=event2store;
		
		std::cout<<"PRINT EVENT2STORAGE\n";
		for(auto a = event2storage.begin();a!=event2storage.end();++a)
		    std::cout<<"storage "<<(a->first)<<"\n";
		std::cout<<"end finishcall no crm for "<<callid<<endl;
	    }
	}
	return "";
}
string Parser::parse_transfercall(string src,string dst,string uid,string timestamp,string callid,string uidcode)
{

	string request = request_str;
	request+=uid;
	request+="&event=1&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst,uidcode);
	request+="&timestamp=";
	request+=timestamp;
	return request;

}
//callqos,userid:18421,time:1519058430,callid:1519058428.32841,rxjitter:0,rxcount:0,txjitter:0,txcount:0,rtt:0,bridged_rxjitter:0,bridged_rxcount:0,bridged_txjitter:0,bridged_txcount:0,bridged_rtt:0
string Parser::parse_qoscall(string uid,string timestamp,string callid,string rxjitter,string rxcount,string txjitter,string txcount,string rtt,string bridged_rxjitter,string bridged_rxcount,string bridged_txjitter,string bridged_txcount,string bridged_rtt)
{
    string request = request_str;
    request+=uid;
    request+="&event=9&call_id=";
    request+=callid;
    request+="&timestamp=";
    request+=timestamp;
    request+="&rxjitter=";
    request+=rxjitter;
    request+="&rxcount=";
    request+=rxcount;
    request+="&txjitter=";
    request+=txjitter;
    request+="&txcount=";
    request+=txcount;
    request+="&rtt=";
    request+=rtt;
    request+="&bridged_rxjitter=";
    request+=bridged_rxjitter;
    request+="&bridged_rxcount=";
    request+=bridged_rxcount;
    request+="&bridged_txjitter=";
    request+=bridged_txjitter;
    request+="&bridged_txcount=";
    request+=bridged_txcount;
    request+="&bridged_rtt=";
    request+=bridged_rtt;
    
    return request;
}

string Parser::parse_onlinepbx(string src,string dst,string uid,string timestamp,string callid,string uidcode,string newname)
{
    boost::replace_all(newname,"u","\\u");
    string request = request_str;
    request+=uid;
    request+="&event=7&call_id=";
    request+=callid;
    request+=format_srcdstnum(src,dst,uidcode);
    request+="&timestamp=";
    request+=timestamp;
    request+="&newwebphonename=";
    request+=newname;
    return request;
}



string Parser::parse_changetree(string uid,string callid,string newtree)
{
    CallRecord call;
    if(currentCalls.getCall(callid,call))
    {
	std::cout<<"SetCurrentTreeID\n";
	call.setCurrentTreeId(newtree);
	currentCalls.updateCall(callid,call);
	
    }
    
    string request = request_str;
    request+=uid;
    request+="&call_id=";
    request+=callid;
    request+="&event=8&newtree=";
    request+=newtree;
    return request;
}

string Parser::parse_gatewaycall(string src,string dst,string callid)
{
    string request = request_str;
    request+="0&event=7&call_id=";
    request+=callid;
    request+="&dst=";
    request+=dst;
    return request;
}

string  Parser::clearStorage(map<string,string>& storage,string key)
{
	auto it = storage.find(key);
	if(it!=storage.end())
	{
		string value = (*it).second;
		storage.erase(it);
		return value;
	}
	
	return "";
}

void Parser::clearCallEnviroment(string callid)
{
/*
    mergedCalls.eraseMergedCall(callid);
    
    clearStorage(event2storage,callid);
    
    clearStorage(callbackIdList,callid);
    
    currentCalls.removeCall(callid);
	
    	
    std::cout<<"EVENT2STORAGE SIZE="<<event2storage.size()<<"\n";
    for(auto it=event2storage.begin();it!=event2storage.end();++it)
	std::cout<<"even2storage "<<(it->first)<<"\n";
    std::cout<<"MERGED CALLS SIZE="<<mergedCalls.getSize()<<"\n";
    mergedCalls.print();
    std::cout<<"CurrentCalls SIZE="<<currentCalls.size()<<"\n";
    currentCalls.print();
*/
}

int Parser::processTransfer(string callid,string linkedid,string record)
{
    boost::mutex::scoped_lock lockTransferStorage(transferstorageLock);
    
    TransferData tmp;
    /*
    tmp.callid=linkedid;
    tmp.recordname=record;
    transferstorage[callid]=tmp;
    */
    tmp.callid=callid;
    tmp.recordname=record;
    transferstorage[linkedid]=tmp;
}

string Parser::parse_cdrevent(string origcallid,string destination,string duration,string billableseconds,string starttime,string endtime,string Destinationcontext)
{
	string request="";
	string transferedCallId = "";
	string callid = mergedCalls.getMergedCall(origcallid);
	
	boost::mutex::scoped_lock lockTransferStorage(transferstorageLock);
	auto x = transferstorage.find(callid);
	if(x!=transferstorage.end())
	{
	    TransferData tmp = x->second;
	    transferedCallId = tmp.callid;
	    string transferrecord = tmp.recordname;
	    
	    std::cout<<"TRANSFERED CALLID "<<callid<<"   "<<transferedCallId<<"  "<<origcallid<<"\n";
	    transferstorage.erase(x);
	    request+="&transfercallid="+callid;
	    request+="&transferrecord="+transferrecord;
	    
	    //callid=transferedCallId;
	}
	//else
	    
	
	lockTransferStorage.unlock();
	
	clearStorage(useridToCallId,callid);
	
	std::cout<<"parse cdr event\n";
	boost::timed_mutex::scoped_lock lockEvent2CDRStorage(event2CDRstorageLock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
	if(!lockEvent2CDRStorage)
	{
	    cout<<"ERROR GET CDR LOCK\n";
	    exit(-1);
	}
	
	std::cout<<"lock accepted\n";
	for(auto a = event2storage.begin();a!=event2storage.end();++a)
	std::cout<<"storage "<<(a->first)<<"\n";
	
	
	auto it = event2storage.find(callid);
/*	if((!transferedCallId.empty())&&(!(it!=event2storage.end())))
	{
		std::cout<<"TRY SET TRANSFERED CALLID \n";
	        it = event2storage.find(transferedCallId);
	}
*/
	request+="&destination="+destination;
	request+="&duration="+duration;
	request+="&billableseconds="+billableseconds;
	request+="&starttime="+starttime;
	request+="&endtime="+endtime;
	request+="&DestinationContext="+Destinationcontext;
	
	std::cout<<"REQUEST prepared = "<<request<<"\n";
	
	if(it!=event2storage.end())
	{
	    boost::mutex::scoped_lock lockReportedCallsStorage(reportedCallstorageLock);
	    auto report = reportedCall.find(callid);
	    if(report!=reportedCall.end())
		return "";
		
	    request = it->second+request;;
	    
	    event2storage.erase(it);
	
	    std::cout<<"cdr event "<<request<<"\n";
	    request+="&parsetask=cdrevent";
	    std::cout<<"ADD REPORTED CALL "<<callid<<"\n";
	    reportedCall[callid]=request;
	    
	}
	else
	{
	    std::cout<<"CDR for callid "<<callid<<" not found\n";
	    std::cout<<"Prepare request and store it\n";
	    //std::cout<<request<<"For callid="<<callid<"\n";
	    std::cout<<"For callid="<<callid<<"\n";
	    event2CDRstorage[callid]=request;
	    request="";
	}
	
	
	return request;
	
}

const CallRecords& Parser::getCallRecords() const
{

	return currentCalls;
}

/*
string Parser::parse_agentcalled(string callid,string agent,string queueid)
{
    
    auto it = useridToCallId.find(callid);
    if(it!=useridToCallId.end())
    {
	string uid = (*it).second;
	string request = request_str;
	request+=uid;
	request+="&event=6&call_id=";
	request+=callid;
	request+="&queueid=";
	request+=queueid;
	request+="&agent=";
	request+=agent;
	
	
	return request;
    }
    return "";
}
*/
/*
boost::timed_mutex::scoped_lock&& Parser::getCDRLock()
{
     boost::timed_mutex::scoped_lock lockEvent2CDRStorage(event2CDRstorageLock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
     if(!lockEvent2CDRStorage)
     {
        lm.makeLog(boost::log::trivial::severity_level::error,"FAILED GET CDR LOCK");
        exit(-1);
     }
     
     return lockEvent2CDRStorage;
}
*/
string Parser::parse_hangupevent(string callid)
{
    std::cout<<"CLEAR FOR HANGUP["<<callid<<"]\n";
    //clearCallEnviroment(callid);
    return "";
}

void Parser::parse_mergecall(string newcallid,string callid)
{
    std::cout<<"merged calls\n";
    mergedCalls.addMergedCall(newcallid,callid);
}

void Parser::parse_setcallbackId(string callid,string callbackId)
{
    callbackIdList[callid] = callbackId;
}



string Parser::fieldNameConverter(std::string fieldname)
{
    if(fieldname.find(":")!=std::string::npos)
	return fieldname;
	
    if(ASTER_VER==13)
    {
//	if(fieldname=="callbackId")
//	    return fieldname;
//	if(fieldname=="webcallid")
//	    return fieldname;
	    
	return fieldname+":";
    }
    return fieldname;
}

string Parser::parsedata(ParserData& data)
{
	lm.makeLog(boost::log::trivial::severity_level::info,"EVENT DATA "+data["Event:"]);
	for(auto it=data.begin();it!=data.end();++it)
	{
	    lm.makeLog(boost::log::trivial::severity_level::info,"DATA "+(it->first)+"  "+(it->second));
	    std::cout<<"DATA ["<<(it->first)<<"]  ["<<(it->second)<<"]\n";
	}
	std::cout<<"\n\n";
	
	string str = "";
	if(data["Event:"]=="UserEvent")
	{
		if(data["UserEvent:"]=="webphonecall")
		{
		    str = parse_webphoneUUID(data[fieldNameConverter("src")],data[fieldNameConverter("dst")],data[fieldNameConverter("userid")],data[fieldNameConverter("time")],data[fieldNameConverter("callid")],data[fieldNameConverter("webcallid")]);
		}
		if(data["UserEvent:"]=="incomecall")
		{
			 str = parse_incomecall(data[fieldNameConverter("src")],data[fieldNameConverter("dst")],data[fieldNameConverter("userid")],data[fieldNameConverter("time")],data[fieldNameConverter("callid")],data[fieldNameConverter("srctype")],data[fieldNameConverter("uidcode")]);
			
		}
		if(data["UserEvent:"]=="queuecall")
		{
			 str = parse_queuecall(data[fieldNameConverter("src")],data[fieldNameConverter("dst")],data[fieldNameConverter("userid")],data[fieldNameConverter("time")],data[fieldNameConverter("callid")],data[fieldNameConverter("srctype")],data[fieldNameConverter("uidcode")]);
			
		}
		if(data["UserEvent:"]=="outcall")
		{
			 str = parse_outcall(data[fieldNameConverter("src")],data[fieldNameConverter("dst")],data[fieldNameConverter("userid")],data[fieldNameConverter("time")],data[fieldNameConverter("callid")],data[fieldNameConverter("uidcode")]);
			
		}
		if(data["UserEvent:"]=="answercall")
		{
			 str = parse_answercall(data[fieldNameConverter("src")],data[fieldNameConverter("dst")],data[fieldNameConverter("userid")],data[fieldNameConverter("time")],data[fieldNameConverter("callid")],data[fieldNameConverter("calltype")],data[fieldNameConverter("usecrm")],data[fieldNameConverter("uidcode")],data[fieldNameConverter("ChannelName")]);
			
		}
		if(data["UserEvent:"]=="initcall")
		{		
			if(!(data[fieldNameConverter("callbackId")]).empty())
			    callbackIdList[data[fieldNameConverter("callid")]] = data[fieldNameConverter("callbackId")];
			    
			std::cout<<"DEBUG initcall "<<data[fieldNameConverter("src")]<<" "<<data[fieldNameConverter("dst")]<<" "<<data[fieldNameConverter("userid")]<<" "<<data[fieldNameConverter("time")]<<" "<<data[fieldNameConverter("callid")]<<" "<<data[fieldNameConverter("recordfile")]<<" "<<data[fieldNameConverter("usecrm")]<<" "<<data[fieldNameConverter("uidcode")]<<"\n";
			 str = parse_initcall(data[fieldNameConverter("src")],data[fieldNameConverter("dst")],data[fieldNameConverter("userid")],
			 data[fieldNameConverter("time")],data[fieldNameConverter("callid")],data[fieldNameConverter("recordfile")],
			 data[fieldNameConverter("usecrm")],data[fieldNameConverter("uidcode")],data[fieldNameConverter("TreeId")],
			 data[fieldNameConverter("ChannelName")],data[fieldNameConverter("roistat")],data[fieldNameConverter("x-roistat-phone")],data[fieldNameConverter("x-roistat-marker")],data[fieldNameConverter("x-callerid")]);
			
		}
		if(data["UserEvent:"]=="transfercall")
		{
			processTransfer(data[fieldNameConverter("Uniqueid:")],data[fieldNameConverter("callid")],data[fieldNameConverter("recordfile")]);
			str = parse_initcall(data[fieldNameConverter("src")],data[fieldNameConverter("dst")],data[fieldNameConverter("userid")],data[fieldNameConverter("time")],
			data[fieldNameConverter("callid")],"",data[fieldNameConverter("usecrm")],data[fieldNameConverter("uidcode")],
			data[fieldNameConverter("TreeId")],data[fieldNameConverter("ChannelName")],data[fieldNameConverter("roistat")],data[fieldNameConverter("x-roistat-phone")],data[fieldNameConverter("x-roistat-marker")],
			data[fieldNameConverter("x-callerid")]);
			
		}
		if(data["UserEvent:"]=="finishcall")
		{
			int skipfinish = 0;
			if(data[fieldNameConverter("callbacktype")].compare("CallBackTreeReverse")==0)
			{
			    std::cout<<"CALLBACKTREEREVERSE\n";
			    
			    CallRecord call;
			    if(currentCalls.getCall(data[fieldNameConverter("callid")],call))
			    {
				 call.setRecordFile(data[fieldNameConverter("recordfile")],true);
				 currentCalls.updateCall(data[fieldNameConverter("callid")],call);
				 
			    }
			    if(!(data[fieldNameConverter("newexten")]).empty())
			    {
				
				
				if(data[fieldNameConverter("dialstatus")].compare("CANCEL")==0)
				{
				    
				    data[fieldNameConverter("callanswer")]="0";
				    data[fieldNameConverter("status")]="NOANSWER";
				    parse_cdrevent(data[fieldNameConverter("callid")],data[fieldNameConverter("newexten")],"0","0",data[fieldNameConverter("callstart")],data[fieldNameConverter("time")],data[fieldNameConverter("callbacktype")]);
				    
				}
				else
				{
				    std::string duration = "0";
				    if((!(data[fieldNameConverter("time")]).empty())&&(!(data[fieldNameConverter("callstart")]).empty()))
					duration =  std::to_string(std::stoi(data[fieldNameConverter("time")]) - std::stoi(data[fieldNameConverter("callstart")]));
				    parse_cdrevent(data[fieldNameConverter("callid")],data[fieldNameConverter("newexten")],duration,duration,data[fieldNameConverter("callstart")],data[fieldNameConverter("time")],data[fieldNameConverter("callbacktype")]);
				}
				data[fieldNameConverter("dst")] = data[fieldNameConverter("newexten")];
			    }
			    else if((data[fieldNameConverter("status")].compare("ANSWERED")!=0)||(data[fieldNameConverter("status")].compare("ANSWER")!=0))
			    {
				data[fieldNameConverter("callanswer")]="0";
				data[fieldNameConverter("status")]="NOANSWER";
				string formdst = data[fieldNameConverter("uidcode")]+"001";
				parse_cdrevent(data[fieldNameConverter("callid")],data[fieldNameConverter("dst")],"0","0",data[fieldNameConverter("callstart")],data[fieldNameConverter("time")],data[fieldNameConverter("callbacktype")]);
				data[fieldNameConverter("src")]=formdst;
			    }
			    else
				skipfinish = 1;
			}
			else if(data[fieldNameConverter("callbacktype")].compare("CallBackReverse")==0)
			{
			//    std::string prevcallid = data["callid"];
			//    data["callid"] = mergedCalls.getParentCall(data["callid"]);
			//    std::cout<<"PREV CALLID = "<<prevcallid<<" new callid "<<data["callid"]<<"\n";
			}
			else if(data[fieldNameConverter("callbacktype")].compare("standart")==0)
			{
			    std::string duration = "0";
			    std::string billsec = "0";

			    if(data[fieldNameConverter("dialstatus")].compare("ANSWER")!=0)
			    {
				string formdst = data[fieldNameConverter("uidcode")]+"001";
				parse_cdrevent(data[fieldNameConverter("callid")],data[fieldNameConverter("dst")],duration,billsec,data[fieldNameConverter("callstart")],data[fieldNameConverter("time")],data[fieldNameConverter("callbacktype")]);
				data[fieldNameConverter("dst")]=formdst;
			    }
			    else
			    {
				if((!(data[fieldNameConverter("time")]).empty())&&(!(data[fieldNameConverter("callstart")]).empty()))
    				    duration =  std::to_string(std::stoi(data[fieldNameConverter("time")]) - std::stoi(data[fieldNameConverter("callstart")]));
				if((!(data[fieldNameConverter("time")]).empty())&&(!(data[fieldNameConverter("callanswer")]).empty())&&(data[fieldNameConverter("callanswer")].compare("0")!=0))
				    billsec =  std::to_string(std::stoi(data[fieldNameConverter("time")]) - std::stoi(data[fieldNameConverter("callanswer")]));
				    
				if((data[fieldNameConverter("dst")]).empty())
				    data[fieldNameConverter("dst")] = data[fieldNameConverter("src")];
				
				parse_cdrevent(data[fieldNameConverter("callid")],data[fieldNameConverter("dst")],duration,billsec,data[fieldNameConverter("callstart")],data[fieldNameConverter("time")],data[fieldNameConverter("callbacktype")]);
			    }
			    
			    
			}
			else if(data[fieldNameConverter("callbacktype")].compare("recall")==0)
			{
			    if(data[fieldNameConverter("status")]=="CHANUNAVAIL")
			    {
				parse_cdrevent(data[fieldNameConverter("callid")],data[fieldNameConverter("dst")],"0","0",data[fieldNameConverter("callstart")],data[fieldNameConverter("time")],data[fieldNameConverter("callbacktype")]);
			    }
			}
			else if(data[fieldNameConverter("callbacktype")].compare("crmredirect")==0)
			{
			    std::string duration = "0";
			    std::string billsec = "0";
			    if((!(data[fieldNameConverter("time")]).empty())&&(!(data[fieldNameConverter("callstart")]).empty()))
    				duration =  std::to_string(std::stoi(data[fieldNameConverter("time")]) - std::stoi(data[fieldNameConverter("callstart")]));
			    if((!(data[fieldNameConverter("time")]).empty())&&(!(data[fieldNameConverter("callanswer")]).empty())&&(data[fieldNameConverter("callanswer")].compare("0")!=0))
				billsec =  std::to_string(std::stoi(data[fieldNameConverter("time")]) - std::stoi(data[fieldNameConverter("callanswer")]));
			    parse_cdrevent(data[fieldNameConverter("callid")],data[fieldNameConverter("dst")],duration,billsec,data[fieldNameConverter("callstart")],data[fieldNameConverter("time")],data[fieldNameConverter("callbacktype")]);
			}
			
			
			if(!skipfinish)    
			    str = parse_finishcall(data[fieldNameConverter("src")],data[fieldNameConverter("dst")],data[fieldNameConverter("userid")],data[fieldNameConverter("time")],
			    data[fieldNameConverter("callid")],data[fieldNameConverter("callstart")],data[fieldNameConverter("callanswer")],data[fieldNameConverter("status")],data[fieldNameConverter("calltype")],data[fieldNameConverter("callbackId")],data[fieldNameConverter("TreeId")],
			    data[fieldNameConverter("ChannelName")],data[fieldNameConverter("serverId")],data[fieldNameConverter("recordfile")],data[fieldNameConverter("label")],data[fieldNameConverter("rating")],data[fieldNameConverter("newstatus")],data[fieldNameConverter("crmcall")],data[fieldNameConverter("hashtag")],data[fieldNameConverter("usecrm")],data[fieldNameConverter("uidcode")]);
			else
			    str = "";
			
		}
		if (data["UserEvent:"] == "finish_transfer")
		{
			str = parse_finishtransfer(data[fieldNameConverter("src")], data[fieldNameConverter("dst")], data[fieldNameConverter("userid")], data[fieldNameConverter("time")], data[fieldNameConverter("callid")],data[fieldNameConverter("uidcode")]);

		}
		if (data["UserEvent:"] == "PickupCall")
		{
		    std::string pickupcmd  = "/var/lib/asterisk/agi-bin/pbxpickupcall.php "+data[fieldNameConverter("channel1")];
		    pickupcmd += " "+data[fieldNameConverter("channel2")];
		    std::cout<<"TRY EXECUTE "<<pickupcmd<<"\n";
		    system(pickupcmd.c_str());
		    
		     
		}
		if(data["UserEvent:"] == "mergecall")
		{
		    parse_mergecall(data[fieldNameConverter("newcallid")],data[fieldNameConverter("callid")]);
		}
		if(data["UserEvent:"] == "onlinepbx")
		{
		    str = parse_onlinepbx(data[fieldNameConverter("src")], data[fieldNameConverter("dst")], data[fieldNameConverter("userid")], data[fieldNameConverter("time")], data[fieldNameConverter("callid")],data[fieldNameConverter("uidcode")],data[fieldNameConverter("newname")]);
		}
		if(data["UserEvent:"] == "callqos")
		{
		// callqos,userid:18421,time:1519058430,callid:1519058428.32841,rxjitter:0,rxcount:0,txjitter:0,txcount:0,rtt:0,bridged_rxjitter:0,bridged_rxcount:0,bridged_txjitter:0,bridged_txcount:0,bridged_rtt:0
		    str = parse_qoscall(data[fieldNameConverter("userid")],data[fieldNameConverter("time")],data[fieldNameConverter("callid")],data[fieldNameConverter("rxjitter")],data[fieldNameConverter("rxcount")],data[fieldNameConverter("txjitter")],data[fieldNameConverter("txcount")],data[fieldNameConverter("rtt")],
		    data[fieldNameConverter("bridged_rxjitter")],data[fieldNameConverter("bridged_rxcount")], data[fieldNameConverter("bridged_txjitter")],data[fieldNameConverter("bridged_txcount")],data[fieldNameConverter("bridged_rtt")]);
		}
		if(data["UserEvent:"] == "changetree")
		{
		    str = parse_changetree(data[fieldNameConverter("userid")],data[fieldNameConverter("callid")],data[fieldNameConverter("newtree")]);
		}    
		if(data["UserEvent:"] == "gatewaycall")
		{
		    str = parse_gatewaycall(data[fieldNameConverter("src")],data[fieldNameConverter("dst")],data[fieldNameConverter("callid")]);
		}
		if((data["callbackId"]).empty())
		{
		    auto x = callbackIdList.find(data[fieldNameConverter("callid")]);
		    if(x!=callbackIdList.end())
		    {
            		data[fieldNameConverter("callbackId")] = (*x).second;
		    }
		}
		
		if(!str.empty())
		{
		    str+= "&callbackId=";
		    str+= data[fieldNameConverter("callbackId")];
		}
	}
	else if(data["Event:"] == "AgentCalled")
	{
		std::cout<<"AGENT CALLED\n\n\n";
	    	string request = request_str;
		CallRecord call;
		string callid = data[fieldNameConverter("Linkedid:")];
		if(currentCalls.getCall(callid,call))
		{
		    std::cout<<"FIND CALL\n";
		    string treeid = call.getCurrentTreeId();
		    if(treeid.empty())
		    {
			lm.makeLog(boost::log::trivial::severity_level::info,"new tree empty");
			std::cout<<"newtree EMPTY\n";
			treeid=call.getTreeId();
		    }
		    data[fieldNameConverter("TreeId")] = treeid;
		    data[fieldNameConverter("ChannelName")] = call.getChannel();
		    auto millis = std::time(0);
		    std::string dstnum="";
		    if(ASTER_VER==13)
		    {
			dstnum="MemberName:";
			lm.makeLog(boost::log::trivial::severity_level::info,data["Interface:"].substr(0,5));
			if(data["Interface:"].substr(0,5)=="PJSIP")
			{
			    lm.makeLog(boost::log::trivial::severity_level::info,"MODIFY dstnum");
			    data[dstnum]=data["Interface:"].substr(13);
			}
			
		    }
		    else
			dstnum="AgentName:";
		    str =  parse_incomecall(data[fieldNameConverter("CallerIDNum:")],data[dstnum],call.getuserid(),std::to_string(millis),callid,call.getsrctype(),call.getuid());
		
		}
		else
		    std::cout<<"NO CALL FOUND\n";
	}
	else if(data["Event:"] == "Cdr")
	{
	    if(data[fieldNameConverter("DestinationContext:")]=="vatscallbackreverse")
	    data[fieldNameConverter("UniqueID:")] = mergedCalls.getMergedCall(data[fieldNameConverter("UniqueID:")]);
		str = parse_cdrevent(data[fieldNameConverter("UniqueID:")],data[fieldNameConverter("Destination:")],data[fieldNameConverter("Duration:")],data[fieldNameConverter("BillableSeconds:")],data[fieldNameConverter("StartTime:")],data[fieldNameConverter("EndTime:")],data[fieldNameConverter("DestinationContext:")]);
	}
/*	else if(data["Event:"] == "Hangup")
	{
		str = parse_hangupevent(data["Uniqueid:"]);
	}*/
/*	else if(data["Event:"] == "AgentCalled")
	{
		str = parse_agentcalled(data["Uniqueid:"],data["AgentName:"],data["Queue:"]);
	}*/
	else
	    return str;
	
	if((!str.empty())&&(data[fieldNameConverter("UserEvent:")]!="changetree")&&(data[fieldNameConverter("UserEvent:")]!="qoscall")&&(data[fieldNameConverter("UserEvent:")]!="finishcall")&&(data[fieldNameConverter("UserEvent:")]!="PickupCall")&&(data[fieldNameConverter("UserEvent:")]!="gatewaycall")&&(data[fieldNameConverter("Event:")]!="Cdr")&&(data[fieldNameConverter("UserEvent:")]!="answercall")&&(data[fieldNameConverter("Event:")]!="Hangup:"))
	{    
		str += "&TreeId=";
		str += data[fieldNameConverter("TreeId")];
		str += "&Channel=";
		str += data[fieldNameConverter("ChannelName")];
		

	}
	if(!str.empty())
	{
	    string requestId = "";
		for(int i = 0; i < 8; ++i) {
	    		requestId += chars[index_dist(rng)];
	            }
		str+="&requestId="+requestId;
	}
	
		
	
	debugParseString(str);
//	std::cout<<"END PROCESS DATA>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
	return str;
}