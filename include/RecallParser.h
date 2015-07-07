#include <IParser.h>
#include <string>

using namespace std;

class RecallParser :public IParser
{
    string parse_recallrequest(string from,string to);
public:
    RecallParser(string requestStr);
    string parsedata(ParamMap& data);
    int parsedata(ParamMap& data,string& from, string& tonum);
};