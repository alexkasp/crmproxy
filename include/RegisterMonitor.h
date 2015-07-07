#include "iexecuter.h"
#include "RegisterParser.h"
#include <DButils.h>

#include <string>
#include <map>

using namespace std;

class RegisterMonitor: public ExecuterInterface
{
	RegisterParser* parser;
	DButils DBWorker;
	
	map<string,string> UidToIdStorage;
    public:
	RegisterMonitor(RegisterParser *_parser,string settings_filename):parser(_parser)
	{
	    if(DBWorker.getAuthParams(settings_filename))
	    {
		DBWorker.connect();
		DBWorker.getUidList(UidToIdStorage);
	    }
	};
	
	virtual int Execute(ParamMap& data);
};