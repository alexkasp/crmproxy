#ifndef IPARSER_INTERFACE
#define IPARSER_INTERFACE 1
#include <string>
#include <map>
#include <iostream>
#include "datatypes.h"

using namespace std;

using ParserData = map < string, string > ;

class IParser
{
	IParser(){};
    protected:
	const string request_str;
	void debugParseString(string str,string module="Unknown")
	{
	    cout<<"Parse result[modul"<<module<<"]:"<<endl<<str<<endl<<endl;                                
	}
    public:
	IParser(const std::string& str):
	request_str(str){};
	
	virtual string parsedata(ParamMap& data) = 0;
};
#endif