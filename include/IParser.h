#ifndef IPARSER_INTERFACE
#define IPARSER_INTERFACE 1
#include <string>
#include <map>

#include "datatypes.h"

using namespace std;

using ParserData = map < string, string > ;

class IParser
{
	IParser(){};
    protected:
	const string request_str;                                
    public:
	IParser(const std::string& str):
	request_str(str){};
	
	virtual string parsedata(ParamMap& data) = 0;
};
#endif