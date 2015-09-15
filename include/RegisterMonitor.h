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
    protected:
    
	virtual int makeAction(ParamMap& data,IParser* currentParser);
    public:
	RegisterMonitor(string settings_filename)
	{
	    if(DBWorker.getAuthParams(settings_filename))
	    {
		DBWorker.connect();
		DBWorker.getUidList(UidToIdStorage);
	    }
	};
	
	
};