#ifndef CRMURL_BUILDER_CLASS
#define CRMURL_BUILDER_CLASS 1
#include "iexecuter.h"


#include <string>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <vector>
#include <ICMServer.h>
#include <CDRManager.h>
#include <DButils.h>
#include <LoggerModule.h>
#include <CurlCallback.h>


class CRMUrlBuilder: public ExecuterInterface
{
	
		
		
	
	CurlCallback curlcb;
	
	virtual	int SendRequest(std::string url);
	int processURL(string url,map<string,string>& CDRData);
    
    protected:
		
		DButils* db;
		CDRManager* cdr;
		ICMServer* icm;
		LoggerModule* lm;
		boost::thread_group tgroup;
		boost::asio::ip::tcp::endpoint ep;
    
        string server;
        string port;
	boost::asio::io_service io;
	
        virtual int makeAction(ParamMap data,IParser* currentParser);
        
	public:
		void sendRequestAndStore(std::string, std::string, std::string);
		CRMUrlBuilder(string server,string port,DButils* _db,ICMServer* _icm = NULL,CDRManager* _cdr = NULL,LoggerModule* _lm = NULL);
		
    
};
#endif