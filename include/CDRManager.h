#ifndef CDR_MANAGER
#define CDR_MANAGER 1
#include <DButils.h>
#include <LoggerModule.h>
#include <map>

class CDRManager
{
    DButils& DBWorker;
    LoggerModule& lm;
    public:
	CDRManager(LoggerModule& lm,DButils& db);
	void processCDR(map<string,string>& data);
};
#endif