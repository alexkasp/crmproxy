#include <RecallManager.h>

RecallManager::RecallManager(RecallParser* setparser)
{
    parser = setparser;
}

int RecallManager::makeAction(ParamMap& data,IParser* parser)
{
    string from;
    string to;
    
    if(parser->parsedata(data,from,to))
    {
        ast.call(from,to);
    }
}