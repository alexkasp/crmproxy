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
		void AcceptRead();
		int SendRequest(std::string url);
		void ReadFunction(const boost::system::error_code &ec,const size_t bytes_transferred);
		void ReadThread();
	public:
		CRMUrlBuilder(Parser parsertostr);
		virtual int Execute(ParamMap& data);
};