#ifndef __AST_MANAGER
#define __AST_MANAGER

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;

class AsteriskManager
{
	

	io_service service;

	std::string asthost;
	int astport;
	protected:
		boost::shared_ptr<ip::tcp::endpoint> ep;
		boost::shared_ptr<ip::tcp::socket> _sock;
		int initConnection(boost::shared_ptr<ip::tcp::socket>& _sock);
		int init();
		int makelogin(const boost::shared_ptr<ip::tcp::socket>& _sock);
		int deinit();
		int softinit();
	public:
		AsteriskManager();
		~AsteriskManager();
		
		int onewaycall(std::string testerschema,std::string testedschema,std::string testid);
		int call(std::string from,std::string to,std::string outline,std::string schema);
		int callsimple(std::string from,std::string to,std::string outline,std::string schema);
		int call(std::string from,std::string to);
		int callWithAnnounce(std::string from,std::string to,std::string announce,std::string simgateway);
		int call(std::string from,std::string to,std::string schema);
		int callCheckAnswer(std::string from,std::string to,std::string channel,std::string callernum,std::string dialstr,std::string dialtime,std::string dialargs);
};
#endif