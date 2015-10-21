#include <ICMServer.h>

ICMServer::ICMServer(LoggerModule& _lm,DButils& _db):socket(service),lm(_lm),db(_db),storage(100000)
{
    
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

void ICMServer::solveRequest(string strData,boost::shared_ptr<ip::udp::endpoint> sender)
{
    string delimiter = ":";
    size_t pos = 0;
    string aNumber;
    string userId;
    string icmMSG = "not found\r\n";

    if((pos = strData.find(delimiter))!= std::string::npos)
    {
        userId = strData.substr(0, pos);
        strData.erase(0, pos + delimiter.length());
        aNumber=strData;
        
        CDRData cdr("Empty","Empty");
        if(storage.getCDRData(userId,aNumber,cdr))
            icmMSG = cdr.operatorNum;
        else if(db.getCallData(userId,aNumber,icmMSG)>0)
        {
    	    int status = 0;
    	    if((status = storage.putCDRData("unknown",userId,aNumber,icmMSG))<1)
    		std::cout<<"ERROR PUT CDR "<<status<<"\n";
        }    
        
    }

    std::cout<<"solveRequest "<<icmMSG<<"\n";
    socket.send_to(boost::asio::buffer(icmMSG),*sender);
}

void ICMServer::storeCDRData(std::map<std::string,std::string>& data)
{
    int status = 0;
    if((status = storage.putCDRData(data))<1)
        std::cout<<"ERROR PUT CDR "<<status<<"\n";
        
}

int ICMServer::putCDREvent(map<string,string>& CDRData)
{
    if (!CDRData.empty()) {
        	if(CDRData["event"]=="2")
        	{
        	    string clientNum,operatorNum;
        	    if(CDRData["calltype"]=="out")
        	    {
        		clientNum = CDRData["dst_num"];
        		operatorNum = CDRData["src_num"];
        		CDRData["dst_num"] = operatorNum;
        		CDRData["src_num"] = clientNum;
        	    }
        	    if((!operatorNum.empty())&&(!clientNum.empty()))
            		storeCDRData(CDRData);
        	}
        	else
        	    return 0;
            return 1;
        }
        return 0;
}

