#pragma once
#include <map>
#include <string>
#include <memory>
#include <queue>
#include <MergedCalls.h>
#include <set>
#include "IParser.h"
#include <boost/thread/mutex.hpp>
#include "UtilDButils.h"
#include <sys/time.h>
#include "FSConnector.h"

using namespace std;

class FSRegStatus
{
    
    public:
    
	static string UNREGSTATUS;
	static string REGSTATUS;
	static string REGISTERSTATUS;
	
	int counter;
	string stateString;
	time_t regTime;
	
	FSRegStatus();
};


class FSParser: public IParser
{
    DButils& DBWorker;
    FSConnector& connector;
    
    string extregid;
    
    map<string,string> registeredList;
    map<string,gatewayData> deletedList;
    map<string,gatewayData> gatewaysList;
    map<string,FSRegStatus> statusMap;
    boost::thread_group tgroup;
    boost::timed_mutex statusMapLock;
    
    void CheckStateCicle();
    void undelLine();
    string gateway_state_parser(ParserData& data);
    
    string unregEvent(string gateway);
    string regEvent(string gateway);
    
public:
	FSParser(const string str,LoggerModule& _lm,DButils& _DBWorker,FSConnector& _connector,std::string _extregid);
	virtual ~FSParser(void);
	string parsedata(ParamMap& data);
};

