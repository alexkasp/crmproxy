#include <Parser.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#undef CALLMANUALCONTROL

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

CallRecord::~CallRecord(void)
{
	cout<<"CallRecords delete for "<<dst<<endl;
}

CallRecord::CallRecord(string _src,string _dst,string _timestamp,string _recordfile):
	dst(_dst),src(_dst),timestamp(_timestamp),recordfile(_recordfile)
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

int CallRecord::setRecordFile(string filename)
{
    if(!filename.empty())
    {
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

void CallRecords::addCall(string callid,string src,string dst,string timestamp,string recordfile)
{
    boost::mutex::scoped_lock lock(CallLock);
    
    auto it=callList.find(callid);
    
    if(it!=callList.end())
	return;
	
    CallRecord cr(src,dst,timestamp,recordfile);
    
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
	
	boost::timed_mutex::scoped_lock lockEvent2CDRStorage(event2CDRstorageLock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
	if(!lockEvent2CDRStorage)
	    exit(-1);
	    
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
	if(!uidcode.empty()&&(num.length()>(uidcode.length()+1)))
	    num_type = ((num.length()<10)&&((num.substr(0,uidcode.length()).compare(uidcode)==0)||((num.substr(1,uidcode.length()).compare(uidcode)==0))))+1;
	else
	    num_type = (num.length()<10)+1;
	
	return boost::lexical_cast<std::string>(num_type);
}
string Parser::format_srcdstnum(string src,string dst,string uidcode)
{
	string result = "&src_num=";
	result+=src;
	result+="&src_type=";
	result+=parse_numtype(src,uidcode);
	result+="&dst_num=";
	result+=dst;
	result+="&dst_type=";
	result+=parse_numtype(dst,uidcode);
	return result;
}
string Parser::parse_initcall(string src,string dst,string uid,string timestamp,string callid,string recordfile,string usecrm,string uidcode)
{

	string request = request_str;
	request+=uid;
	request+="&event=1&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst,uidcode);
	request+="&timestamp=";
	request+=timestamp;
	
	useridToCallId[callid]=uid;
	
	CallRecord call;
	if(!currentCalls.getCall(callid,call))
	{
	    currentCalls.addCall(callid,src,dst,timestamp,recordfile);
	}
	else
	{
	    std::cout<<"SET RECORDFILE = "<<recordfile<<endl;
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
    request+="&dst";
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

string Parser::parse_answercall(string src,string dst,string uid,string timestamp,string callid,string calltype,string usecrm,string uidcode)
{
	//int src_type = (src.length()<10)+1;
	//int dst_type = (dst.length()<10)+1;
	
	string request = request_str;
	request+=uid;
	request+="&event=3&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst,uidcode);
	request+="&timestamp=";
	request+=timestamp;
	if(usecrm=="1")
	{
	    return request;
	}
	else
	{
	    request="nocrm"+request;
	}
	
	
	string realcallid = mergedCalls.getMergedCall(callid);
	CallRecord call;
	if(currentCalls.getCall(realcallid,call))
	{
	    call.addNumber(dst);
	}
	
	
	return request;
}

string Parser::parse_finishcall(string src,string dst,string uid,string timestamp,string callid,string callstart,string callanswer,string status,string calltype, string callbackId,string treeid, string channel,string serverId,string recordfile,string label,string rating,string usecrm,string uidcode)
{
	std::cout<<"START FINISHCALL\n";
	string event2store;
	
	string request = request_str;
	
	CallRecord call;
	if(currentCalls.getCall(callid,call))
	{
	    std::cout<<"find call "<<call.getcallid()<<" with time "<<call.gettimestamp()<<"\n";
	    callstart = call.gettimestamp();
	    //if(recordfile.empty())
	    recordfile = call.getrecordfile();
	}
	else
	return "";
	
	request+=uid;
	request+="&uidcode=";
	request+=uidcode;
	request+="&call_id=";
	request+=callid;
	request+=format_srcdstnum(src,dst,uidcode);
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
	
	event2store=request;
	event2store+="&event=2";
	
	request+="&event=4";
	
	event2store+="&callbackId=";
	event2store+=callbackId;
	
	boost::timed_mutex::scoped_lock lockEvent2CDRStorage(event2CDRstorageLock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
	
	if(!lockEvent2CDRStorage)
	    exit(-1);
	
	auto cdrdata = event2CDRstorage.find(callid);
	
	if(cdrdata!=event2CDRstorage.end())
	{
	    
	    int involvedNums = call.removeNumber(dst);
	    if(involvedNums==0)
	    {
		std::cout<<"PREPARED REQUEST=\n"<<(cdrdata->second)<<"\n";
	        event2store+=cdrdata->second;
	    
		
	        //clearCallEnviroment(callid);
	        event2CDRstorage.erase(cdrdata);
	        event2store+="&parsertask=finish";
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

string Parser::parse_cdrevent(string callid,string destination,string duration,string billableseconds,string starttime,string endtime,string Destinationcontext)
{
	clearStorage(useridToCallId,callid);
	callid = mergedCalls.getMergedCall(callid);
	
	std::cout<<"parse cdr event\n";
	boost::mutex::scoped_lock lockEvent2Storage(event2storageLock);
	
	std::cout<<"lock accepted\n";
	for(auto a = event2storage.begin();a!=event2storage.end();++a)
	std::cout<<"storage "<<(a->first)<<"\n";
	
	auto it = event2storage.find(callid);
	
	string request="";
	
	request+="&destination="+destination;
	request+="&duration="+duration;
	request+="&billableseconds="+billableseconds;
	request+="&starttime="+starttime;
	request+="&endtime="+endtime;
	request+="&DestinationContext="+Destinationcontext;
	
	if(it!=event2storage.end())
	{
	    request = it->second+request;;
	    
	    event2storage.erase(it);
	
	    std::cout<<"cdr event "<<request<<"\n";
	    request+="&parsetask=cdrevent";
	}
	else
	{
	    boost::timed_mutex::scoped_lock lockEvent2CDRStorage(event2CDRstorageLock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
	    if(!lockEvent2CDRStorage)
		exit(-1);
		
	    std::cout<<"CDR for callid "<<callid<<" not found\n";
	    std::cout<<"Prepare request \n"<<request<<"For callid="<<callid<"\n";
	    //boost::mutex::scoped_lock lockEvent2Storage(event2storageLock);
	    event2CDRstorage[callid]=request;
	    request="";
	}
	
	
	return request;
	
}

const CallRecords& Parser::getCallRecords() const
{

	return currentCalls;
}

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

string Parser::parsedata(ParserData& data)
{
	for(auto it=data.begin();it!=data.end();++it)
	    lm.makeLog(boost::log::trivial::severity_level::info,"DATA "+(it->first)+"  "+(it->second));
	string str = "";
	if(data["Event:"]=="UserEvent")
	{
		if(data["UserEvent:"]=="webphonecall")
		{
		    str = parse_webphoneUUID(data["src"],data["dst"],data["userid"],data["time"],data["callid"],data["webcallid"]);
		}
		if(data["UserEvent:"]=="incomecall")
		{
			 str = parse_incomecall(data["src"],data["dst"],data["userid"],data["time"],data["callid"],data["srctype"],data["uidcode"]);
			
		}
		if(data["UserEvent:"]=="outcall")
		{
			 str = parse_outcall(data["src"],data["dst"],data["userid"],data["time"],data["callid"],data["uidcode"]);
			
		}
		if(data["UserEvent:"]=="answercall")
		{
			 str = parse_answercall(data["src"],data["dst"],data["userid"],data["time"],data["callid"],data["calltype"],data["usecrm"],data["uidcode"]);
			
		}
		if(data["UserEvent:"]=="initcall")
		{
			if(!(data["callbackId"]).empty())
			    callbackIdList[data["callid"]] = data["callbackId"];
			    
			 str = parse_initcall(data["src"],data["dst"],data["userid"],data["time"],data["callid"],data["recordfile"],data["usecrm"],data["uidcode"]);
			
		}
		if(data["UserEvent:"]=="transfercall")
		{
			str = parse_initcall(data["src"],data["dst"],data["userid"],data["time"],data["callid"],"","",data["uidcode"]);
			
		}
		if(data["UserEvent:"]=="finishcall")
		{
			 str = parse_finishcall(data["src"],data["dst"],data["userid"],data["time"],data["callid"],data["callstart"],data["callanswer"],data["status"],data["calltype"],data["callbackId"],data["TreeId"],data["ChannelName"],data["serverId"],data["recordfile"],data["label"],data["rating"],data["usecrm"],data["uidcode"]);
			
		}
		if (data["UserEvent:"] == "finish_transfer")
		{
			str = parse_finishtransfer(data["src"], data["dst"], data["userid"], data["time"], data["callid"],data["uidcode"]);

		}
		if(data["UserEvent:"] == "mergecall")
		{
		    parse_mergecall(data["newcallid"],data["callid"]);
		}
		
		if((data["callbackId"]).empty())
		{
		    auto x = callbackIdList.find(data["callid"]);
		    if(x!=callbackIdList.end())
		    {
            		data["callbackId"] = (*x).second;
		    }
		}
		
		if(!str.empty())
		{
		    str+= "&callbackId=";
		    str+= data["callbackId"];
		}
	}
	else if(data["Event:"] == "Cdr")
	{
		str = parse_cdrevent(data["UniqueID:"],data["Destination:"],data["Duration:"],data["BillableSeconds:"],data["StartTime:"],data["EndTime:"],data["DestinationContext:"]);
	}
/*	else if(data["Event:"] == "Hangup")
	{
		str = parse_hangupevent(data["Uniqueid:"]);
	}*/
	else if(data["Event:"] == "AgentCalled")
	{
		str = parse_agentcalled(data["Uniqueid:"],data["AgentName:"],data["Queue:"]);
	}
	else
	    return str;
	
	if((!str.empty())&&(data["Event:"]!="Cdr")&&(data["Event:"]!="answercall")&&(data["Event:"]!="Hangup:"))
	{    
		str += "&TreeId=";
		str += data["TreeId"];
		str += "&Channel=";
		str += data["ChannelName"];
		

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