#include "iexecuter.h"
#include "RegisterParser.h"
#include <DButils.h>

#include <string>
#include <map>

using namespace std;

class RegisterMonitor: public ExecuterInterface
{
	RegisterParser* parser;
	DButils& DBWorker;
	
	map<string,string> UidToIdStorage;
    protected:
    
	virtual int makeAction(ParamMap& data,IParser* currentParser);
    public:
	RegisterMonitor(DButils& db):DBWorker(db)
	{
		DBWorker.getUidList(UidToIdStorage);
	};
	
	
};