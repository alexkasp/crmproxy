#include "iexecuter.h"

void ExecuterInterface::AddParser(IParser* parsertostr)
{
    parser.push_back(parsertostr);
}

int ExecuterInterface::Execute(ParamMap& data)
{
    auto x = parser.begin();
    for (auto currentParser = x; currentParser != parser.end(); ++currentParser)
    {
         makeAction(data,*currentParser);
    }
    return 0;
}