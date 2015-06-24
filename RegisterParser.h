#include "IParser.h"

#include <string>

using namespace std;

class RegisterParser: public IParser
{
	string parse_peerstatus(string peer,string status,string address);
    public:
	string parsedata(ParamMap& data);
	int parsedata(ParamMap& data,string& number,string& status,string& address);
	RegisterParser(string str);
};