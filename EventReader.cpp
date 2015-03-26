#include "EventReader.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include "iexecuter.h"



using namespace std;

void EventReader::SetExecuter(ExecuterInterface* iexecuter)
{
	Executer = iexecuter;
}
EventReader::EventReader(std::string host,int port):asthost(host),astport(port)
{
	Executer = nullptr;
}


EventReader::~EventReader(void)
{
}



int EventReader::start(void)
{

	ip::tcp::endpoint ep(ip::address::from_string(asthost), astport);
	ip::tcp::socket _sock(service);
	
	connect(_sock,ep);
	_sock.write_some(buffer("Action: login\nUsername: crmproxy\nSecret: mycode\n"));
	
	char bufdata[4096];
	int bytes = 0;
	
	_sock.write_some(buffer("Action: Events\nEventmask: on\n"));
	
	while(1)
	{
		memset(bufdata,0,4096);
		{
			boost::system::error_code ec;
			bytes = _sock.read_some(boost::asio::buffer(bufdata),ec);
			if(ec)
			{
				while(!connect(_sock,ep))
				{
					boost::this_thread::sleep_for(boost::chrono::milliseconds(10000));
				}
				continue;
			}
			else
				processevent(bufdata);
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
	
	cout<<"parseline:"<<line<<endl;
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
			cout<<"start send"<<endl;
			for(auto x=data.begin();x!=data.end();++x)
			{
				cout<<"key->"<<(*x).first<<endl;
				cout<<"value->"<<(*x).second<<endl<<endl;
			}
			/*string request = parsestr.parsedata(data);
			if(request.size()>0)
			{
				cout<<"try to send"<<endl<<request<<endl;
				this->SendRequest(request);
				cout<<request<<endl;
			}*/
			if(Executer!=nullptr)
				Executer->Execute(data);
			if(data.empty())
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
		cout<<"process data"<<endl<<data<<endl;
		int state = 0;//try find event
		int currentevent = 0;
		std::vector<std::string> lines;
		boost::algorithm::split(lines, data, boost::is_any_of("\n"));
		for(auto x = lines.begin();x!=lines.end();++x)
		{
			parseline(*x,state,currentevent);
		}
		cout<<"end process data"<<endl;
	return 0;
}


int EventReader::AddParam(std::string data, ParamMap& eventdata)
{
	std::string key;
	std::string value;
	std::istringstream in(data);
	in >> key >> value;
	if(value=="")
	{
		try
		{
			std::vector<std::string> lines;
			boost::algorithm::split(lines, data, boost::is_any_of(":"));
			auto x = lines.begin();
			key = *x++;
			value = *x;
			value.pop_back();
		}
		catch (exception& e)
		  {
			cout<<"CATCH EXCEPTION!!!" << e.what() << '\n';
		  }
	}
	
	eventdata[key]=value;
	cout << "receive:"<<key<<value<<endl;
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
