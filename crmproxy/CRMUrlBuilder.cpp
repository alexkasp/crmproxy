#include <CRMUrlBuilder.h>
#include <CurlCallback.h>
#include <boost/bind.hpp>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <curl/curl.h>

CRMUrlBuilder::CRMUrlBuilder(string webserver,string _port,DButils* _db,ICMServer* _icm,CDRManager* _cdr,LoggerModule *_lm):icm(_icm),cdr(_cdr),lm(_lm)
{
    server = webserver;
    port = _port;
    boost::asio::ip::tcp::resolver resolver(io);
    boost::asio::ip::tcp::resolver::query query(server,port);
    
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve( query);
    ep = *iter;
    std::cout << ep.address().to_string() << std::endl;
    db = _db;
    
    
    
};

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    CurlCallback curlcb;
    return curlcb.callback(ptr,size, nmemb,userdata);
}

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


int CRMUrlBuilder::makeAction(ParamMap rawdata,IParser* currentParser)
{
    map<string,string> data;
    try{
    
	
    
    string httpsign = "nocrm";
    
	string request = currentParser->parsedata(rawdata);
	if (!request.empty())
        {
	    if(processURL(request,data))
	    {
		
		
		if(request.compare(0,httpsign.length(),httpsign)!=0)
    		{
    		    if(db!=NULL)
    			db->addSendEventReportEntry(data["call_id"],data["requestId"],data["serverId"],data["userId"],"2",request);
    		    
    		    std::cout<<"sendRequestAndStore\n";
    		    sendRequestAndStore(request,data["requestId"],data["call_id"]);
    		    std::cout<<"END sendRequestAndstore\n";
		}
		std::cout<<"check icm and run\n";
		if(icm!=NULL)
		    icm->putCDREvent(data);
		std::cout<<"check cdr and run\n";
        	if(cdr!=NULL)
        	    cdr->processCDR(data);
        	std::cout<<"complete makeAction\n";
            }
            return 1;
        }
    }
     catch(exception &ec)
         {
        	string errmsg = "Error in makeAction ";
//        	lm.makeLog(boost::log::trivial::severity_level::error,errmsg+ec.what());
		std::cout<<errmsg<<"Error in makeAction  "<<ec.what()<<"\n";
		
		for(auto x = data.begin();x!=data.end();++x)
		{
		    std::cout<<"DATA WITH ERROR"<<((*x).first)<<"    "<<((*x).second)<<"\n";
		}
             }
             
    return 0;
}

void CRMUrlBuilder::sendRequestAndStore(string url,string requestId,string callid)
{
     CURL *curl;
       CURLcode res;

      /* get a curl handle */
    curl = curl_easy_init();
     if(curl) {
     /* First set the URL that is about to receive our POST. This URL can
     just as well be a https:// URL if that is what should receive the
                data. */
    string curlBaseUrl = "http://"+server+":"+port+url;
    
    std::cout<<"curlBaseUrl "<<curlBaseUrl<<"\n";
    
    curl_easy_setopt(curl, CURLOPT_URL,curlBaseUrl.c_str());


    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    callbackParam* sendParam =new callbackParam(db,requestId,callid,lm);


    curl_easy_setopt(curl, CURLOPT_WRITEDATA, sendParam);

     /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
     /* Check for errors */
      if(res != CURLE_OK)
         fprintf(stderr, "curl_easy_perform() failed: %s\n",
      curl_easy_strerror(res));
      curl_easy_cleanup(curl);
    }
    std::cout<<"send url complete\n";
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
