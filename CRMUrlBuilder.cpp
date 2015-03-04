#include "CRMUrlBuilder.h"
#include <boost/bind.hpp>

#include <curl/curl.h>
CRMUrlBuilder::CRMUrlBuilder(Parser parsertostr):parser(parsertostr)
{
    boost::asio::ip::tcp::resolver resolver(io);
    boost::asio::ip::tcp::resolver::query query("sipuni.com","80");
    
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve( query);
    ep = *iter;
    std::cout << ep.address().to_string() << std::endl;

    
    
    
};

void CRMUrlBuilder::AcceptRead()
{
    tgroup.create_thread(boost::bind(&CRMUrlBuilder::ReadThread,this));
}

void CRMUrlBuilder::ReadThread()
{
    io.run();
}

void CRMUrlBuilder::ReadFunction(const boost::system::error_code &ec,const size_t bytes_transferred)
{
    boost::asio::streambuf buffer;
//    if(!ec)
//	boost::asio::read_until(sock,buffer,"\n");
}

int CRMUrlBuilder::Execute(ParamMap& data)
{
	string request = parser.parsedata(data);
	if(request.size()>0)
		return SendRequest(request);
	else
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

/*	
	CURL *curl;
	CURLcode res;
 
	curl = curl_easy_init();
	if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
   
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    */
    /* Perform the request, res will get the return code */ 
    //res = curl_easy_perform(curl);
    /* Check for errors */ 
    //if(res != CURLE_OK)
      //fprintf(stderr, "curl_easy_perform() failed: %s\n",
        //      curl_easy_strerror(res));
 
    /* always cleanup */ 
    //curl_easy_cleanup(curl);
//	return 1;
  //}
	return 0;
}