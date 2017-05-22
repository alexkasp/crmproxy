#ifndef TEST_PARSER
#define TEST_PARSER
#include <IParser.h>
#include <string>

using namespace std;

class TestParser :public IParser
{
    string parse_testrequest(string testid);
public:
    TestParser(string requestStr,LoggerModule& lm);
    string parsedata(ParamMap& data);
    int parsedata(ParamMap& data,string& callid,string& testid);
};
#endif