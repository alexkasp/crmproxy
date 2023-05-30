#include "astmanager.h"
#include <iostream>

void keepAliveActivate(boost::asio::ip::tcp::socket* tcpsocket)
{
	// the timeout value
    unsigned int timeout_milli = 10000;

    // platform-specific switch
#if defined _WIN32 || defined WIN32 || defined OS_WIN64 || defined _WIN64 || defined WIN64 || defined WINNT
  // use windows-specific time
  int32_t timeout = timeout_milli;
  setsockopt(tcpsocket->native(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
  setsockopt(tcpsocket->native(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
#else
  // assume everything else is posix
  struct timeval tv;
  tv.tv_sec  = timeout_milli / 1000;
  tv.tv_usec = timeout_milli % 1000;
//  setsockopt(tcpsocket->native(), SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
//  setsockopt(tcpsocket->native(), SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
#endif
}


int AsteriskManager::makelogin(const boost::shared_ptr<ip::tcp::socket>& _sock)
{
    
    boost::asio::streambuf response;
    boost::asio::read_until(*_sock, response, "\r\n");
    boost::asio::read_until(*_sock, response, "\r\n");
  std::string command = "Action: login\r\nUsername: myasterisk\r\nSecret: mycode\r\nActionID: 1\r\n\r\n";
    _sock->write_some(buffer(command,command.size()));
        
    boost::asio::read_until(*_sock, response, "\r\n");
    return 1;
}


int AsteriskManager::init()
{
    return initConnection(_sock);
}

int AsteriskManager::initConnection(boost::shared_ptr<ip::tcp::socket>& _tmpsock)
{
    try{
        ep.reset(new ip::tcp::endpoint( ip::address::from_string(asthost), astport));
         _tmpsock.reset(new ip::tcp::socket(service));
         
         if(!_sock)
            std::cout<<"INIT CONNECTION SOCK EMTY\n";
    	 if(!_tmpsock)
    	    std::cout<<"INIT CONNECTION TMPSOCK EMTY\n";
         
        // keepAliveActivate(_sock.get());
        _tmpsock->connect(*ep); 
        return makelogin(_tmpsock);
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
    
    return 1;
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
	if(!_sock)
	    std::cout<<"SOCKET EMPTY\n";
	
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

int AsteriskManager::callWithAnnounce(std::string from,std::string to,std::string announce,std::string simgateway, std::string callernum, std::string user)
{
	try
	{
	    softinit();
	    std::string command = "Action: Originate\r\nChannel: Local/"+from+"@vatsrecall\r\nExten: announce:"+announce+":"+to+"\r\nContext: vatsoutrecall\r\nPriority: 1\r\nCallerID: "+
		    from+"\r\nVariable: callernum="+callernum+",CALLERID(dnid)="+to+",client="+to+",SimGateWay="+simgateway+",RecallAnnounce="+announce+",userid="+user+"\r\nActionID: 2\r\n\r\n";
    	    std::cout<<command<<"\n";
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

int AsteriskManager::callCheckAnswer(std::string from,std::string to,std::string channel,std::string callernum,std::string callid,std::string dialtime,std::string dialargs)
{
	try
	{
	    
	    std::cout<<"START callCheckAnswer\n";
	    boost::shared_ptr<ip::tcp::socket> _sock;
	    initConnection(_sock);
	    std::cout<<"INITATING callwithcheckanswer...\n";
	    
	    std::string command = "Action: Originate\r\nChannel: Local/"+to+"@callcheckanswer\r\nApplication: musiconhold\r\nCallerID: "+from+"\r\nVariable: CHECKANSWERFUNK=1,CHECKANSWERCALLER="+callernum+",CALLBACKCHANNEL="+channel+",CHECKANSWERCALLID="+callid+",CHECKANSWERDIALTIME="+dialtime+",CHECKANSWERDIALARGS="+dialargs+"\r\nActionID: "+to+"\r\n\r\n";
    	    std::cout<<command<<"\n";
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


int AsteriskManager::onewaycall(std::string testerschema,std::string testedschema,std::string testid)
{
    try
	{
		init();
	std::string command = "Action: Originate\r\nChannel: Local/"+testerschema+"@vatsl\r\nExten: "+testedschema+"\r\nContext: vatsl\r\nPriority: 1\r\nVariable: Testcall="+testid+"\r\nActionID: 2\r\n\r\n";
    
	_sock->write_some(buffer(command,command.size()));
	}
	catch (std::exception& e)
		  {
			std::cout<<"CATCH EXCEPTION!!! AsteriskManager::onewaycall(std::string schema)" << e.what() << '\n';
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
