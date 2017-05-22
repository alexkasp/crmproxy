#include <TestParser.h>
#include <iexecuter.h>
#include <astmanager.h>
#include <DButils.h>

class TestManager: public ExecuterInterface
{
	DButils* db;
        TestParser* parser;
        AsteriskManager ast;
        virtual int makeAction(ParamMap data,IParser* currentParser);
       public:
       TestManager(DButils* _db);
       
};