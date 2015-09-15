#include <RecallParser.h>
#include <iexecuter.h>
#include <astmanager.h>

class RecallManager: public ExecuterInterface
{
        RecallParser* parser;
        AsteriskManager ast;
        const int WaitForSeconds = 10;
        virtual int makeAction(ParamMap& data,IParser* currentParser);
	void callWithPause(std::string from,std::string to,std::string announce);
    public:
        RecallManager();
    
};