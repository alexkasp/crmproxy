#include <MonitorParser.h>
#include <map>
#include <string>
#include <DButils.h>
#include <MergedCalls.h>

using namespace std;

class MonitorParserCRM: public MonitorParser
{
	MergedCalls mergedCalls;
	DButils DBWorker;
	map<string,int> crmUsers;
	int initCrmUsers(map<string,int>& users);
	void refreshCrmUsersList();
	
	virtual string parse_cdrevent(string uniqueID,string accountCode);
    public:
	string parsedata(ParamMap& data);
	MonitorParserCRM(string requeststr,string workerStr,LoggerModule& lm);
};