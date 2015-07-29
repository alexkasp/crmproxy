#ifndef MONITOR_PARSER
#define MONITOR_PARSER 1

#include <IParser.h>
#include <string>

using namespace std;

class MonitorParser :public IParser
{
    virtual string parse_cdrevent(string uniqueID,string accountCode);
public:
    MonitorParser(string requestStr);
    string parsedata(ParamMap& data);
};
#endif