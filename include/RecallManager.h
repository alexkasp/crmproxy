#include <RecallParser.h>
#include <iexecuter.h>
#include <astmanager.h>

class RecallManager: public ExecuterInterface
{
	boost::thread_group tgroup;
	
        RecallParser* parser;
        AsteriskManager ast;
        const int WaitForSeconds = 10;
        virtual int makeAction(ParamMap data,IParser* currentParser);
	void callWithPause(std::string from, std::string to, std::string announce, std::string simgateway);
	void callCheckAnswer(std::string from,std::string to,std::string channel,std::string callernum,std::string dialstr,std::string dialtime,std::string dialargs);
    public:
        RecallManager();
    
};