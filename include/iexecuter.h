#include "datatypes.h"
#include "IParser.h"
#include <vector>



#ifndef __EXECUTER_INTERFACE
#define __EXECUTER_INTERFACE
class ExecuterInterface
{
       std::vector<IParser*> parser;
    protected:
        virtual int makeAction(ParamMap data,IParser*) = 0;
	public:
        void AddParser(IParser* parsertostr);
		virtual int Execute(ParamMap& data);

};
#endif