#ifndef ICM_SERVER
#define ICM_SERVER

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <string>

using namespace boost::asio;
using namespace std;

class ICMServer
{
    io_service service;
    ip::udp::socket socket;
    
    void prepareAccept();
    void processICM(const boost::system::error_code& error,std::size_t bytes_transferred, boost::shared_ptr<ip::udp::endpoint> sender,boost::shared_ptr<boost::asio::streambuf> databuf);
    void solveRequest(string number,boost::shared_ptr<ip::udp::endpoint> sender);
public:
    int init(int port);
    void startProcessing();
    
};

#endif