#ifndef CRMURL_BUILDER_CLASS
#define CRMURL_BUILDER_CLASS 1
#include "iexecuter.h"


#include <string>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <vector>
#include <ICMServer.h>
#include <CDRManager.h>

class CRMUrlBuilder: public ExecuterInterface
{
	
		
		
	
    
	virtual	int SendRequest(std::string url);
	int processURL(string url,map<string,string>& CDRData);
    
    protected:
		CDRManager* cdr;
		ICMServer* icm;
		boost::thread_group tgroup;
		boost::asio::ip::tcp::endpoint ep;
    
        string server;
	boost::asio::io_service io;
        virtual int makeAction(ParamMap& data,IParser* currentParser);
        
	public:
		CRMUrlBuilder(string server,string port,ICMServer* _icm = NULL,CDRManager* _cdr = NULL);
		
    
};
#endif