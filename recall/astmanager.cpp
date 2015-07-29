#include "astmanager.h"

int AsteriskManager::init()
{
    try{
        ep.reset(new ip::tcp::endpoint( ip::address::from_string(asthost), astport));
         _sock.reset(new ip::tcp::socket(service));
        _sock->connect(*ep);
         boost::asio::streambuf response;
        boost::asio::read_until(*_sock, response, "\r\n");
        boost::asio::read_until(*_sock, response, "\r\n");

        std::string command = "Action: login\r\nUsername: myasterisk\r\nSecret: mycode\r\nActionID: 1\r\n\r\n";
        _sock->write_some(buffer(command,command.size()));
        
        boost::asio::read_until(*_sock, response, "\r\n");
        return 1;	
    }
    catch(std::exception &e)
    {
        std::cout<<"CATCH EXCEPTION!!! AsteriskManager::init()" << e.what() << '\n';
        return 0;
    }
}


int AsteriskManager::softinit()
{
    try
    {
        boost::asio::streambuf response;
        std::string command = "Action: login\r\nUsername: myasterisk\r\nSecret: mycode\r\nActionID: 1\r\n\r\n";
        _sock->write_some(buffer(command,command.size()));
        boost::asio::read_until(*_sock, response, "\r\n");
        return 1;
    }
    catch(std::exception &e)
    {
        std::cout<<"CATCH EXCEPTION!!! AsteriskManager::init()" << e.what() << '\n';
        return 0;
    }
}

int AsteriskManager::deinit()
{
	std::string command = "Action: logoff\r\n\r\n";
	_sock->write_some(buffer(command,command.size()));
	
	_sock->close();
	return 0;
}

AsteriskManager::~AsteriskManager()
{
	deinit();
}
AsteriskManager::AsteriskManager()
{
	astport=5038;
	asthost = "127.0.0.1";

	init();
	
}

int AsteriskManager::call(std::string from,std::string to,std::string outline,std::string schema)
{
	try{
		boost::asio::streambuf response;
		init();
	std::string command = "Action: Originate\r\nChannel: SIP/"+to+"@"+outline+"\r\nExten: "+schema+"\r\nContext: vatscallback\r\nPriority: 1\r\nCallerID: "+from+"\r\nVariable: CALLERID(dnid)="+to+",CALLERID(num)="+to+"\r\nActionID: 2\r\n\r\n";
    
	_sock->write_some(buffer(command,command.size()));
	boost::asio::read_until(*_sock, response, "\r\n");
	//deinit();
	}
	catch (std::exception& e)
		  {
			std::cout<<"CATCH EXCEPTION!!! AsteriskManager::call(std::string from,std::string to,std::string outline,std::string schema)" << e.what() << '\n';
		  }
}

int AsteriskManager::callsimple(std::string from,std::string to,std::string outline,std::string schema)
{
	try{
		boost::asio::streambuf response;
		init();
	std::string command = "Action: Originate\r\nChannel: Local/"+to+"@vatsautocallsimple\r\nExten: "+schema+"\r\nContext: vatscallback\r\nPriority: 1\r\nCallerID: "+from+"\r\nVariable: CALLERID(dnid)="+to+",Outline="+outline+",CALLERID(num)="+to+"\r\nActionID: 2\r\n\r\n";
    
	_sock->write_some(buffer(command,command.size()));
	boost::asio::read_until(*_sock, response, "\r\n");
	//deinit();
	}
	catch (std::exception& e)
		  {
			std::cout<<"CATCH EXCEPTION!!! AsteriskManager::call(std::string from,std::string to,std::string outline,std::string schema)" << e.what() << '\n';
		  }
}

int AsteriskManager::call(std::string from,std::string to)
{
	try
	{
	    softinit();
	    std::string command = "Action: Originate\r\nChannel: Local/"+from+"@vatsrecall\r\nExten: "+to+"\r\nContext: vatsout\r\nPriority: 1\r\nCallerID: "+from+"\r\nVariable: CALLERID(dnid)="+to+"\r\nActionID: 2\r\n\r\n";
    
	    boost::system::error_code ec;
	    
	    boost::asio::streambuf response;
	    _sock->write_some(buffer(command,command.size()),ec);
	    
	    if(!ec)
	    {
		boost::asio::read_until(*_sock, response, "\r\n");
	    }
	    else
	    {
		while(!init())
		{
		    boost::this_thread::sleep_for(boost::chrono::milliseconds(10000));
		}
		
		    _sock->write_some(buffer(command,command.size()),ec);
	    }
	}
	catch (std::exception& e)
		  {
			std::cout<<"CATCH EXCEPTION!!! AsteriskManager::call(std::string from,std::string to)" << e.what() << '\n';
		  }
}

int AsteriskManager::call(std::string from,std::string to,std::string schema)
{
	try
	{
		init();
	std::string command = "Action: Originate\r\nChannel: Local/"+schema+"@vatsreserveschemastart\r\nExten: "+to+"\r\nContext: vatsreserveschema\r\nPriority: 1\r\nCallerID: "+from+"\r\nVariable: CALLERID(dnid)="+to+"\r\nActionID: 2\r\n\r\n";
    
	_sock->write_some(buffer(command,command.size()));
	}
	catch (std::exception& e)
		  {
			std::cout<<"CATCH EXCEPTION!!! AsteriskManager::call(std::string from,std::string to,std::string schema)" << e.what() << '\n';
		  }
}