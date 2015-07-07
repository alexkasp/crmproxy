#include <RecallManager.h>

RecallManager::RecallManager(RecallParser* setparser)
{
    parser = setparser;
}

int RecallManager::Execute(ParamMap& data)
{
    string from;
    string to;
    
    if(parser->parsedata(data,from,to))
    {
        //call
    }
}