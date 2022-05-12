#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#ifdef __linux__
#include <unistd.h>
#endif // __linux__




#include <iostream>


#define VERSION "1.1 with agent called"

using namespace boost::asio;

int main()
{
	
	cout<<"VERSION "<<VERSION<<endl;

	io_service service;
	ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 5038);
	ip::tcp::socket _sock(service);
	_sock.connect(ep);
	
	_sock.write_some(buffer("Action: login\nUsername: crmproxy\nSecret: mycode\n"));
	//char buf[1024];
	char bufdata[1024];
	int bytes = 0;
	//int bytes = read(_sock, buffer(buf));
	//std::cout<<buf<<std::endl;
	boost::this_thread::sleep( boost::posix_time::millisec(4000));

	
	_sock.write_some(buffer("Action: Events\nEventmask: on\n"));
	boost::this_thread::sleep( boost::posix_time::millisec(4000));
	if ( _sock.available())
		{
			bytes = _sock.read_some(boost::asio::buffer(bufdata));
			std::cout<<bufdata;
		}


	while(1)
	{
		
		memset(bufdata,0,1024);
		if ( _sock.available())
		{
			bytes = _sock.read_some(boost::asio::buffer(bufdata));
			std::cout<<bufdata;
		}
		//std::string msg(buff);


	/*	if ( msg.find("login ") == 0)
		{
			std::string tmpauth;
			std::istringstream in(msg);
	
			in >> tmpauth >> tmpauth;
		}*/
	}
	_sock.write_some(buffer("_Action: Events\nEventmask: off\n"));
	_sock.write_some(buffer("Action: logoff\n"));
	_sock.close();
	service.run();
	return 0;
}