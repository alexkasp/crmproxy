#include <CRMUrlBuilder.h>
#include <boost/bind.hpp>
#include <iostream>

CRMUrlBuilder::CRMUrlBuilder(string webserver,string port)
{
    server = webserver;
    boost::asio::ip::tcp::resolver resolver(io);
    boost::asio::ip::tcp::resolver::query query(server,port);
    
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve( query);
    ep = *iter;
    std::cout << ep.address().to_string() << std::endl;

    
    
    
};



int CRMUrlBuilder::makeAction(ParamMap& data,IParser* currentParser)
{
	string request = currentParser->parsedata(data);
	if (!request.empty())
        {
	    SendRequest(request);
            //tgroup.create_thread(boost::bind(&CRMUrlBuilder::SendRequest,this,request));
            return 1;
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