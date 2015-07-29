#ifndef CRMURL_BUILDER_CLASS
#define CRMURL_BUILDER_CLASS 1
#include "iexecuter.h"


#include <string>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <vector>


class CRMUrlBuilder: public ExecuterInterface
{
	
		
		
	
    
		int SendRequest(std::string url);
    
    
    protected:
		boost::thread_group tgroup;
		boost::asio::ip::tcp::endpoint ep;
    
        string server;
	boost::asio::io_service io;
        virtual int makeAction(ParamMap& data,IParser* currentParser);
        
	public:
		CRMUrlBuilder(string server,string port);
		
    
};
#endif