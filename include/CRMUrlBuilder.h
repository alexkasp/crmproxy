#include "iexecuter.h"
#include "IParser.h"

#include <string>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <vector>


class CRMUrlBuilder: public ExecuterInterface
{
	
		boost::asio::io_service io;
		std::vector<IParser*> parser;
		boost::thread_group tgroup;
		boost::asio::ip::tcp::endpoint ep;
		
		const string server="sipuni.com";
		
		int SendRequest(std::string url);

		
	public:
		CRMUrlBuilder();
		void AddParser(IParser* parsertostr);
		virtual int Execute(ParamMap& data);
};