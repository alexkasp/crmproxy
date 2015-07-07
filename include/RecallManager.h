#include <RecallParser.h>
#include <iexecuter.h>

class RecallManager: public ExecuterInterface
{
    RecallParser* parser;
    
    public:
    RecallManager(RecallParser* setparser);
    virtual int Execute(ParamMap& data);
};