#include <DButils.h>
#include <CRMUrlBuilder.h>
#include <CDRManager.h>
#include <sys/time.h>
#include <vector>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

class WSSMonitor
{
    boost::asio::io_service service;
    boost::thread_group tgroup;
    
    DButils& db;
    CRMUrlBuilder& crm;
    CDRManager& cdr;
	const string logfile = "/var/log/wssmonitor.log";
	void WeHaveEventButNoCDR(std::string);
	void WeSendEventButNoAnswer(std::string);
	void WeHaveCDRButNotSendEvent(string origcallid,string callid);
    public:
	WSSMonitor(DButils& _db,CRMUrlBuilder& _crm,CDRManager& _cdr);
	void unstop();
	void start();
	void Check();
    
};