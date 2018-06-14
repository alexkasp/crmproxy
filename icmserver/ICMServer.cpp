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
    std::cout<<"We wait for connect\n";
}

void ICMServer::prepareAccept()
{
    //boost::shared_ptr<boost::asio::streambuf> buf = boost::shared_ptr<boost::asio::streambuf>(new boost::asio::streambuf());
    
    std::cout<<"PREPARE ACCEPT\n";
    while(1)
    {
	std::cout<<"getRequest from prepareaccept\n";
        getRequest();
        std::cout<<"make service.run from PREPAREACCEPT\n";
        service.run();
    }
    std::cout<<"END PREPARE ACCEPT\n";
}

void ICMServer::processICM(const boost::system::error_code& error,std::size_t bytes_transferred, boost::shared_ptr<ip::udp::endpoint> sender,char* databuf)
{
    try{
    
	
	
	if(!error)
	{
    	    getRequest();
    //	    std::cout<<"MAKE PAUSE HERE \n";
//	int pause;
//	std::cin>>pause;
        
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
    string icmMSG = "";

    if((pos = strData.find(delimiter))!= std::string::npos)
    {
        userId = strData.substr(0, pos);
        strData.erase(0, pos + delimiter.length());
        aNumber=convertNumber(strData);
        
        CDRData cdr("Empty","Empty","Empty");
        if(storage.getCDRData(userId,aNumber,cdr))
            icmMSG = cdr.operatorNum;
        else if(db.getCallData(userId,aNumber,icmMSG)>0)
        {
    	    int status = 0;
    	    if((status = storage.putCDRData("unknown",userId,aNumber,icmMSG,"fromDB"))<1)
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

string ICMServer::convertNumber(string num)
{
    const int numLimit = 10;
    if(num.length()>9)
        return num.substr(num.length()-numLimit,numLimit);
    return num;    	    
}

int ICMServer::putCDREvent(map<string,string> CDRData)
{
        	    for(auto x=CDRData.begin();x!=CDRData.end();++x)
        	     std::cout<<(x->first)<<" : "<<(x->second)<<"\n";
    
    std::cout<<"ICMServer "<<CDRData["status"]<<" event = "<<CDRData["event"]<<"\n";
    if ((!CDRData.empty())&&((CDRData["status"]=="ANSWER")||(CDRData["status"]=="ANSWERED"))) {
        	if(CDRData["event"]=="2")
        	{
        	    for(auto x=CDRData.begin();x!=CDRData.end();++x)
        	     std::cout<<(x->first)<<" : "<<(x->second)<<"\n";
        	    
        	    string clientNum,operatorNum;
        	    if(CDRData["calltype"]=="out")
        	    {
        		clientNum = CDRData["dst_num"];
        		operatorNum = CDRData["src_num"];
        		CDRData["dst_num"] = operatorNum;
        		CDRData["src_num"] = clientNum;
        	    }
        	    
        	    std::cout<<"ICMServer start storeCDRdata\n";
        	     
        	     
        	    CDRData["src_num"] = convertNumber(CDRData["src_num"]);
        	    clientNum = CDRData["dst_num"];
        	    operatorNum = CDRData["src_num"];
        	            
        	    if((!operatorNum.empty())&&(!clientNum.empty()))
            		storeCDRData(CDRData);
            	    else
            		std::cout<<"Problem with numbers - operatornum = "<<operatorNum<<" clientNum = "<<clientNum<<"\n";
        	}
        	else
        	    return 0;
            return 1;
        }
        return 0;
}

