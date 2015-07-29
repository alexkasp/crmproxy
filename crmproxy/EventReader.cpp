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
EventReader::EventReader(std::string host,int port):asthost(host),astport(port)
{
	//Executer = nullptr;
}


EventReader::~EventReader(void)
{
    for(auto i=Executer.begin();i!=Executer.end();++i)
	delete (*i);

}



int EventReader::start(void)
{

	ip::tcp::endpoint ep(ip::address::from_string(asthost), astport);
	ip::tcp::socket _sock(service);
	
	connect(_sock,ep);
	
	char databuf[1024];
	boost::asio::streambuf buf;
	int bytes = 0;
		
	while(1)
	{
		{
			boost::system::error_code ec;
			bytes = boost::asio::read_until(_sock,buf,"\r\n\r\n",ec);
			if (!ec)
			{
			    string str(boost::asio::buffers_begin(buf.data()), boost::asio::buffers_begin(buf.data()) + buf.size());
			    
			
			    buf.consume(bytes);
			    processevent(str);
			    
			}
			else
			{
				while(!connect(_sock,ep))
				{
					boost::this_thread::sleep_for(boost::chrono::milliseconds(10000));
				}
				continue;
			}
		}
	}
		
	_sock.write_some(buffer("Action: Events\nEventmask: off\n"));
	_sock.write_some(buffer("Action: logoff\n"));
	_sock.close();
	return 0;
}

int EventReader::parseline(string line,int& state,int& event)
{
	if(line.find("Event:")==0)
	{
		state = 1;	
	}
	else if((line.size()<2)&&(state==2))
	{
		state = 3;
		cout<<"prepare to send"<<endl;
	}
	
	//cout<<"parseline:"<<line<<endl;
	//cout<<"go switch"<<endl;
	switch(state)
	{
		case(1):
		{
			AddParam(line,data);
			state = 2;
			break;
		}
		case(2):
		{
			AddParam(line,data);
			break;	
		}
		case(3):
		{
			/*cout<<"start send"<<endl;
			for(auto x=data.begin();x!=data.end();++x)
			{
				cout<<"key->"<<(*x).first<<endl;
				cout<<"value->"<<(*x).second<<endl<<endl;
			}*/
			/*string request = parsestr.parsedata(data);
			if(request.size()>0)
			{
				cout<<"try to send"<<endl<<request<<endl;
				this->SendRequest(request);
				cout<<request<<endl;
			}*/
			for(auto i=Executer.begin();i!=Executer.end();++i)
			    (*i)->Execute(data);
			if(!data.empty())
				data.clear();
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
		int state = 0;//try find event
		int currentevent = 0;
		std::vector<std::string> lines;
		boost::algorithm::split(lines, data, boost::is_any_of("\n"));
		    for(auto x = lines.begin();x!=lines.end();++x)
		    {
			parseline(*x,state,currentevent);
		    }
		}
		catch(exception& e)
		{
		    cout<<"process parseline error "<<e.what()<<endl;
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
			cout<<"ADDPARAM  EXCEPTION!!!" << e.what() << '\n';
		  }
	}else if(string::npos == data.find(':'))
	{
	    cout<<"CATCH! "<<data<<endl;
	    
	}
	
	eventdata[key]=value;
	//cout << "receive:"<<key<<value<<endl;
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
