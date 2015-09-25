#include <ICMServer.h>

ICMServer::ICMServer(LoggerModule& _lm):socket(service),lm(_lm),IParser("EMPTY",_lm)
{
    
}

void ICMServer::parse_finishcall(string src,string dst,string callid)
{

}

void ICMServer::parse_mergecall(string newcallid,string callid)
{

}


void ICMServer::parse_cdrevent(string callid)
{

}

string ICMServer::parsedata(ParamMap& data)
{

    if(data["Event:"]=="UserEvent")
    {
	if(data["UserEvent:"]=="finishcall")
	{
	    parse_finishcall(data["src"],data["dst"],data["callid"]);
	}
	if(data["UserEvent:"] == "mergecall")
	{
	    parse_mergecall(data["newcallid"],data["callid"]);
	}
    }
    else if(data["Event:"] == "Cdr")
    {
    	parse_cdrevent(data["UniqueID:"]);
    }
    return "";
}

int ICMServer::init(int port)
{
    socket.open(ip::udp::v4());
    boost::system::error_code ec;
    socket.bind(ip::udp::endpoint(ip::udp::v4(),port),ec);
        
    if(!ec)
        return 1;
    return 0;
}
            
void ICMServer::startProcessing()
{
    boost::thread t1(boost::bind(&ICMServer::prepareAccept,this));
    t1.detach();
}

void ICMServer::getRequest()
{
    char* buf = new char[128];
    
    boost::shared_ptr<ip::udp::endpoint> sender = boost::shared_ptr<ip::udp::endpoint>(new ip::udp::endpoint());
    socket.async_receive_from(boost::asio::buffer(buf,128),*sender,boost::bind(&ICMServer::processICM,this,_1,_2,sender,buf));
}

void ICMServer::prepareAccept()
{
    //boost::shared_ptr<boost::asio::streambuf> buf = boost::shared_ptr<boost::asio::streambuf>(new boost::asio::streambuf());
    
    while(1)
    {
	getRequest();
	service.run();
    }
}

void ICMServer::processICM(const boost::system::error_code& error,std::size_t bytes_transferred, boost::shared_ptr<ip::udp::endpoint> sender,char* databuf)
{
    try{
    
	if(!error)
	{
    	    getRequest();
        
    	    databuf[bytes_transferred]=0;
        
    	    string str(databuf);
        
    	    boost::thread solveThread(boost::bind(&ICMServer::solveRequest,this,str,sender));
    	    solveThread.detach();
        
	}
    }
    catch(exception &ec)
    {
    	string errmsg = "Error in read_handle ";
    	lm.makeLog(boost::log::trivial::severity_level::error,errmsg+ec.what());
    }
    delete[] databuf;
}

void ICMServer::solveRequest(string number,boost::shared_ptr<ip::udp::endpoint> sender)
{
    string icmMSG = "not found\r\n";
    socket.send_to(boost::asio::buffer(icmMSG),*sender);
}