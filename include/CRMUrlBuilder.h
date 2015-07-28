#include "iexecuter.h"


#include <string>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <vector>


class CRMUrlBuilder: public ExecuterInterface
{
	
		boost::asio::io_service io;
		
		boost::thread_group tgroup;
		boost::asio::ip::tcp::endpoint ep;
    
        string server;
    
		
    
    
    protected:
        virtual int makeAction(ParamMap& data,IParser* currentParser);
        int SendRequest(std::string url);
	public:
		CRMUrlBuilder(string server,string port);
		
    
};