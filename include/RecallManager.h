#include <RecallParser.h>
#include <iexecuter.h>
#include <astmanager.h>

class RecallManager: public ExecuterInterface
{
        RecallParser* parser;
        AsteriskManager ast;
        virtual int makeAction(ParamMap& data,IParser* currentParser);
    public:
        RecallManager();
    
};