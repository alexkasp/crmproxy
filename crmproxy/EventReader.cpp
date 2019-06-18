#include <EventReader.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include "iexecuter.h"



using namespace std;

void EventReader::AddExecuter(ExecuterInterface* iexecuter)
{
	Executer.push_back(iexecuter);
}
EventReader::EventReader(std::string host,int port,LoggerModule& _lm):asthost(host),astport(port),lm(_lm),_sock(service),ep(ip::address::from_string(asthost), astport)
{
	servhost = host;
	servport = port;
	//ip::tcp::endpoint ep(ip::address::from_string(asthost), astport);
	//ip::tcp::socket _sock(service);
}


EventReader::~EventReader(void)
{
    for(auto i=Executer.begin();i!=Executer.end();++i)
	delete (*i);

}




int EventReader::startProcessing(std::string str,std::string& prev)
{
    lm.makeLog(info,"GET BUFFER, START PARSE\n["+str+"]\n");
    try
        {
	    std::vector<std::string> lines;
	    //boost::algorithm::split(lines, str, boost::is_any_of("\r\n\r\n"));
	    boost::iter_split(lines, str, boost::algorithm::first_finder("\r\n\r\n"));
	    for(auto x = lines.begin();x!=lines.end();++x)
	    {
	        std::string value = *x;
	        std::size_t found = value.find(getMark());
	        if((found==std::string::npos)&&(found==0))
	        {
		    value=prev+"\n"+value;
		    lm.makeLog(info,"CORRECTING ...:\n ["+(value)+"]");
		}
		else
		{
		    prev = value;
		}
		if(!(*x).empty())
		{
		    lm.makeLog(info,"AMI:\n ["+(value)+"]");
		    boost::thread t(boost::bind(&EventReader::processevent,this,value));
		    tgroup.add_thread(&t);
		    t.detach();
		}	
	    }
	}
	catch(exception& e)
	{
	    string errmsg = "READHANDLER  EXCEPTION!!!";
	    lm.makeLog(boost::log::trivial::severity_level::error,errmsg+e.what());
	    return 0;
	}
    return 1;
}

void EventReader::read_handler(boost::shared_ptr<boost::asio::streambuf> databuf,const boost::system::error_code& ec,std::size_t size)
{
    boost::mutex::scoped_lock Lock(handleReceiveLock);
    string prev;
    try{
	
	if (!ec)
	{
	    
	    boost::asio::streambuf& buf = *databuf;
	    
	    string str(boost::asio::buffers_begin(buf.data()), boost::asio::buffers_begin(buf.data()) + buf.size());

	    buf.consume(size);
	    
	    
	    //processevent(str,data);
	    readRequest();
	}
	else
	{
	    
	    std::cout<<ec.category().name() << ':' << ec.value();
	    
	    string errmsg = ec.category().name() + ec.value();
	    lm.makeLog(boost::log::trivial::severity_level::error,"NetWork ERROR: "+errmsg);
	}
    }
    catch(exception &ec)
    {
	string errmsg = "Error in read_handle ";
	lm.makeLog(boost::log::trivial::severity_level::error,errmsg+ec.what());
    }
    
}

void EventReader::readRequest()
{
//    boost::shared_ptr<boost::asio::streambuf> buf = boost::shared_ptr<boost::asio::streambuf>(new boost::asio::streambuf());
//    boost::asio::async_read_until(_sock,*buf,"\r\n\r\n",boost::bind(&EventReader::read_handler,this,buf,_1,_2));
    string prev = "";
    while(1)
    {
	std::string data;
	size_t n = boost::asio::read_until(_sock,boost::asio::dynamic_buffer(data), "\r\n\r\n");
	std::string line = data.substr(0, n);
	if(!startProcessing(line,prev))
	    return;
	    
	data.erase(0, n);
    }
}

int EventReader::start(void)
{

	
	
	connect(_sock,ep);
	
	
	while(1)
	{
		readRequest();
		service.run();
		lm.makeLog(boost::log::trivial::severity_level::error,"Disconnect from ATS");
		//tgroup.join_all();
		lm.makeLog(info,"Start reconnect");
			
		while(!connect(_sock,ep))
		{
			boost::this_thread::sleep_for(boost::chrono::milliseconds(10000));
		}
		lm.makeLog(info,"Reconnect SUCCESS");
		service.reset();
	}
		
	_sock.write_some(buffer("Action: Events\nEventmask: off\n"));
	_sock.write_some(buffer("Action: logoff\n"));
	_sock.close();
	return 0;
}

string EventReader::getMark()
{
    return "Event:";
}

int EventReader::parseline(string line,int& state,int& event,ParamMap& structdata)
{
	string startEventMark = getMark();
	if(line.find(startEventMark)==0)
	{
		state = 1;	
	}
	else if((line.size()==0)&&(state==2))
	{
		state = 3;
	}
	
	switch(state)
	{
		case(1):
		{
			AddParam(line,structdata);
			state = 2;
			break;
		}
		case(2):
		{
			AddParam(line,structdata);
			break;	
		}
		case(3):
		{
			for(auto i=Executer.begin();i!=Executer.end();++i)
			{
			    (*i)->Execute(structdata);
			}
			state = 0;
			break;
		}
		default:
			break;
	}
	return 0;
}
int EventReader::processevent(const std::string data)
{
		try{
		
		    ParamMap structdata;
		    int state = 0;//try find event
		    int currentevent = 0;
		    std::vector<std::string> lines;
		    boost::algorithm::split(lines, data, boost::is_any_of("\n"));
		    
		    for(auto x = lines.begin();x!=lines.end();++x)
		    {
			parseline(*x,state,currentevent,structdata);
		    }
		    parseline("",state,currentevent,structdata);
		}
		catch(exception& e)
		{
		    string errmsg = "process line error";
		    lm.makeLog(boost::log::trivial::severity_level::error,errmsg+e.what());
		}
	return 0;
}


int EventReader::AddParam(std::string data, ParamMap& eventdata)
{
	std::string key;
	std::string value;
	std::istringstream in(data);
	in >> key >> value;
	if((value=="")&&( string::npos != data.find(':')))
	{
		try
		{
			std::vector<std::string> lines;
			boost::algorithm::split(lines, data, boost::is_any_of(":"));
			auto x = lines.begin();
			key = *x++;
			value = *x;
			if(!value.empty())
			    value.pop_back();
		}
		catch (exception& e)
		  {
			string errmsg = "ADDPARAM  EXCEPTION!!!";
			lm.makeLog(boost::log::trivial::severity_level::error,errmsg+e.what());
		  }
	}else if(string::npos == data.find(':'))
	{
	    lm.makeLog(warning,"CATCH! "+data);
	    
	}
	
	eventdata[key]=value;
//	cout << "receive:"<<key<<value<<endl;
	return 0;
}

int EventReader::connect(boost::asio::ip::tcp::socket& socket, boost::asio::ip::tcp::endpoint& ep)
{
	try{
		if(socket.is_open())
		{
			socket.close();
			boost::this_thread::sleep_for(boost::chrono::milliseconds(10000));
		}
		
		socket.connect(ep);
		socket.write_some(buffer("Action: login\nUsername: crmproxy\nSecret: mycode\n"));
		socket.write_some(buffer("Action: Events\nEventmask: on\n"));
	}
	catch(exception &ec)
	{
		cout << ec.what() << endl;
		return 0;
	}
	return 1;
}
