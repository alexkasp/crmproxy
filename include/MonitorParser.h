#ifndef MONITOR_PARSER
#define MONITOR_PARSER 1

#include <IParser.h>
#include <string>

#include <MergedCalls.h>

using namespace std;

class MonitorParser :public IParser
{
    MergedCalls mergedCalls;
    virtual string parse_cdrevent(string uniqueID,string accountCode);
protected:
    void parse_mergecall(string newcallid,string callid);
public:
    MonitorParser(string requestStr, LoggerModule& lm);
    string parsedata(ParamMap& data);
};
#endif