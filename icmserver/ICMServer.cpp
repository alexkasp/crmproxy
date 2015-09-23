#include <ICMServer.h>

ICMServer::ICMServer(int port):socket(io_serice)
{
    
}

int ICMServer::init(int port)
{
    socket.open(ip::udp::v4();
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

void ICMServer::prepareAccept()
{
    boost::shared_ptr<boost::asio::streambuf> buf = boost::shared_ptr<boost::asio::streambuf>(new boost::asio::streambuf());
    
    boost::shared_ptr<ip::udp::endpoint> sender = boost::shared_ptr<ip::udp::endpoint>(new ip::udp::endpoint());
    socket.async_receive(boost::asio::buffer(*buf),*sender,boost::bind(&ICMServer::pro));
    io_serice.run();
}

void ICMServer::processICM(const boost::system::error_code& error,std::size_t bytes_transferred, boost::shared_ptr<ip::udp::endpoint> sender,boost::shared_ptr<boost::asio::streambuf> databuf)
{
    
    if(!error)
    {
        prepareAccept();
        
        boost::asio::streambuf& buf = *databuf;
        ip::udp::endpoint sender_ept = *sender;
        
        string str(boost::asio::buffers_begin(buf.data()), boost::asio::buffers_begin(buf.data()) + buf.size());
        
        buf.consume(size);
        
        boost::thread solveThread(boost::bind(&ICMServer::solveRequest,this,str,sender_ept));
        
    }
}

                void ICMServer::solveRequest(string number,boost::shared_ptr<ip::udp::endpoint> sender)
                {
                    string icmMSG = "not found\r\n";
                    socket.send_to(boost::asio::buffer(icmMSG),*sender);
                }