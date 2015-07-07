#include <RecallParser.h>
#include <iexecuter.h>
#include <astmanager.h>

class RecallManager: public ExecuterInterface
{
    RecallParser* parser;
    AsteriskManager ast;
    
    public:
    RecallManager(RecallParser* setparser);
    virtual int Execute(ParamMap& data);
};