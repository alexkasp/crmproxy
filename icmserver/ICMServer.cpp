#include <ICMServer.h>
#include <boost/algorithm/string.hpp>

ICMServer::ICMServer(LoggerModule& _lm):socket(service),lm(_lm)
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

void ICMServer::solveRequest(string number,boost::shared_ptr<ip::udp::endpoint> sender)
{
    string icmMSG = "not found\r\n";
    socket.send_to(boost::asio::buffer(icmMSG),*sender);
}

void ICMServer::storeCDRData(std::map<std::string,std::string>& data)
{
    for(auto x=data.begin();x!=data.end();++x)
    {
        std::cout<<(x->first)<<" -- "<<(x->second)<<"\n";
    }
}

int ICMServer::putCDREvent(string url)
{
    try {
        std::map<std::string,std::string> CDRData;
        
        std::string delimiter = "=";
        
        std::vector<std::string> lines;
        boost::algorithm::split(lines, url, boost::is_any_of("?"));
        
        if(lines.size()==2)
        {
            std::vector<std::string> params;
            
            boost::algorithm::split(params, lines.at(1), boost::is_any_of("&"));
            for(auto x = params.begin();x!=params.end();++x)
            {
                std::string data = *x;
                size_t pos = 0;
                if((pos = data.find(delimiter))!= std::string::npos)
                {
                    std::string param = data.substr(0, pos);
                    data.erase(0, pos + delimiter.length());
                    std::string value=data;
                    CDRData[param] = value;
                    
                }
                
            }
            if (!CDRData.empty()) {
        	if(CDRData["event"]=="2")
            	    storeCDRData(CDRData);
            }
            
        }
        
    } catch (exception& e) {
        string errmsg = "ICM parse URL error "+url;
        lm.makeLog(boost::log::trivial::severity_level::error,errmsg+e.what());
	std::cout<<errmsg<<" "<<e.what()<<"\n";
    }
}