#include <IParser.h>
#include <string>

using namespace std;

class RecallParser :public IParser
{
    string parse_recallrequest(string from,string to);
public:
    RecallParser(string requestStr,LoggerModule& lm);
    string parsedata(ParamMap& data);
    int parsedata(ParamMap& data,string& from, string& tonum,string& announce, string& simgateway, string& callername, string& user);
    int parsedatacheckanswer(ParamMap& data,string& from,string& to,string& channel,string& callernum,string& dialstr,string& dialtime,string& dialargs);
};