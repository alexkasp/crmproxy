#pragma once



#ifdef __linux__
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread/mutex.hpp>
#elif __APPLE__
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread/mutex.hpp>
#else
#include <boost\asio.hpp>
#include <boost\thread.hpp>
#include <boost\algorithm\string\split.hpp>
#include <boost\algorithm\string.hpp>
#include <boost\thread\mutex.hpp>
#endif

#include <vector>

#include <string>
#include "datatypes.h"
#include "iexecuter.h"
#include <LoggerModule.h>

using namespace boost::asio;


class EventReader
{
	std::string asthost;
	int astport;
	io_service service;
	std::vector<ExecuterInterface*> Executer;
	LoggerModule& lm;
protected:
	
	string servhost;
	int servport;
	
	virtual string getMark();
	
	int processevent(const std::string data);
public:
	EventReader(std::string host,int port,LoggerModule& lm);
	~EventReader(void);
	int start(void);
private:
	ip::tcp::endpoint ep;
	ip::tcp::socket _sock;
	boost::thread_group tgroup;
	boost::mutex handleReceiveLock;
	std::string prevdata;
	
	void writeHandler(const boost::system::error_code& error, std::size_t bytes_transferred);
	void readRequest();
	void read_handler(boost::shared_ptr<boost::asio::streambuf> databuf,const boost::system::error_code& ec,std::size_t size);
	int connect(boost::asio::ip::tcp::socket& socket, boost::asio::ip::tcp::endpoint& ep);
	int SendRequest(std::string url);
	int parseline(std::string line,int& _state,int& _event,ParamMap& structdata);
	
public:
	void AddExecuter(ExecuterInterface* iexecuter);
	int AddParam(std::string data, ParamMap& eventdata);
};

