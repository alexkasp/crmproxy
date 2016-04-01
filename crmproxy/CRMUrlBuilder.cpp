#include <CRMUrlBuilder.h>
#include <boost/bind.hpp>
#include <iostream>
#include <boost/algorithm/string.hpp>

CRMUrlBuilder::CRMUrlBuilder(string webserver,string port,ICMServer* _icm,CDRManager* _cdr):icm(_icm),cdr(_cdr)
{
    server = webserver;
    boost::asio::ip::tcp::resolver resolver(io);
    boost::asio::ip::tcp::resolver::query query(server,port);
    
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve( query);
    ep = *iter;
    std::cout << ep.address().to_string() << std::endl;

    
    
    
};

int CRMUrlBuilder::processURL(string url,map<string,string>& CDRData)
{
    try {
        
        
        std::string delimiter = "=";
        
        std::vector<std::string> lines;
        boost::algorithm::split(lines, url, boost::is_any_of("?"));
        
        //std::cout<<"processURL "<<url<<endl;
        
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
            	    //std::cout<<"processURL split params "<<data<<std::endl;
            	    
                    std::string param = data.substr(0, pos);
                    data.erase(0, pos + delimiter.length());
                    std::string value=data;
                    CDRData[param] = value;
                    //std::cout<<"processURL split params complete"<<param<<" = "<<value<<std::endl;
                }
                
            }
            
            return 1;
        }
    } catch (exception& e) {
        string errmsg = "ICM parse URL error "+url;
        //lm.makeLog(boost::log::trivial::severity_level::error,errmsg+e.what());
	std::cout<<errmsg<<" "<<e.what()<<"\n";
    }
    
    return 0;
}


int CRMUrlBuilder::makeAction(ParamMap& data,IParser* currentParser)
{
    try{
    string httpsign = "nocrm";
    
	string request = currentParser->parsedata(data);
	if (!request.empty())
        {
    	    if(request.compare(0,httpsign.length(),httpsign)!=0)
    	    {
		SendRequest(request);
	    }

	    map<string,string> data;
	    
	    if(processURL(request,data))
	    {
		if(icm!=NULL)
		    icm->putCDREvent(data);
        	if(cdr!=NULL)
        	    cdr->processCDR(data);
            }
            return 1;
        }
    }
     catch(exception &ec)
         {
        	string errmsg = "Error in makeAction ";
//        	lm.makeLog(boost::log::trivial::severity_level::error,errmsg+ec.what());
		std::cout<<errmsg<<"Error in makeAction  "<<ec.what()<<"\n";
             }
             
    return 0;
}

int CRMUrlBuilder::SendRequest(std::string url)
{
	
	try{
	    
	    boost::asio::streambuf request;
	    
	    std::ostream request_stream(&request);
	    
	    request_stream << "GET " << url << " HTTP/1.0\r\n";
	    request_stream << "Host: " << server << "\r\n";
	    request_stream << "Accept: */*\r\n";
	    request_stream << "Connection: close\r\n\r\n";
	    
	    boost::asio::ip::tcp::socket sock(io);
	    sock.connect(ep);
	    boost::system::error_code ec;
	    boost::asio::write(sock,request);
	    sock.close();
    
	}
	catch (exception& e)
	{
	    cout<<"CATCH EXCEPTION!!!" << e.what() << '\n';
	}
	return 0;
}
