#include <MonitorParser.h>

class MonitorParserCRM: public MonitorParser
{
	virtual string parse_cdrevent(string uniqueID,string accountCode);
    public:
	MonitorParserCRM(string requeststr):MonitorParser(requeststr)
	{}
};