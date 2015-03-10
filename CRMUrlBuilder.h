#include "iexecuter.h"
#include "Parser.h"

#include <string>

#include <boost/asio.hpp>
#include <boost/thread.hpp>


class CRMUrlBuilder: public ExecuterInterface
{
	
		boost::asio::io_service io;
		Parser parser;
		boost::thread_group tgroup;
		boost::asio::ip::tcp::endpoint ep;
		
		const string server="sipuni.com";
		
		int SendRequest(std::string url);

		
	public:
		CRMUrlBuilder(Parser parsertostr);
		virtual int Execute(ParamMap& data);
};