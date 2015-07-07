#include <IParser.h>
#include <string>

using namespace std;

class RecallParser :public IParser
{

public:
    RecallParser(string requestStr);
    string parsedata(ParamMap& data);
    int parsedata(ParamMap& data,string& from, string& tonum);
};