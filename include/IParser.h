#ifndef IPARSER_INTERFACE
#define IPARSER_INTERFACE 1
#include <string>
#include <map>
#include <iostream>
#include <LoggerModule.h>
#include "datatypes.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 

using namespace std;

using ParserData = map < string, string > ;

class IParser
{
	
	//IParser(){};
    protected:
	LoggerModule &lm;
	const string request_str;
	void debugParseString(string str,string module="Unknown")
	{
	    if(!str.empty())
	    {
		lm.makeLog(info,"["+module+"]:"+str);                                
		//cout<<"DebugINFO "<<str<<endl;
		//boost::this_thread::sleep( boost::posix_time::milliseconds(60000));
	    }
	}
    public:
	IParser(const std::string str,LoggerModule& _lm):
	request_str(str),lm(_lm){};
	
	virtual string parsedata(ParamMap& data) = 0;
};
#endif