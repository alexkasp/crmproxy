#include <WSSMonitor.h>


WSSMonitor::WSSMonitor(DButils& _db,CRMUrlBuilder& _crm,CDRManager& _cdr):db(_db),crm(_crm),cdr(_cdr)
{}

void WSSMonitor::start()
{
    tgroup.create_thread(boost::bind(&WSSMonitor::Check,this));
}

void WSSMonitor::WeHaveCDRButNotSendEvent(string origcallid,string callid)
{
    std::cout<<"WeHaveCDRButNotSendEvent origcallid = "<<origcallid<<" callid = "<<callid<<"\n";
    string cmd = "echo WeHaveCDRButNotSendEvent callid ="+callid+ ">>"+logfile;
    system(cmd.c_str());
}

void WSSMonitor::WeHaveEventButNoCDR(string request)
{
    std::cout<<"WeHaveEventButNoCDR request = "<<request<<"\n";
    string cmd = "echo WeHaveEventButNoCDR request = "+request+">>"+logfile;
    system(cmd.c_str());
}

void WSSMonitor::WeSendEventButNoAnswer(string request)
{
    std::cout<<"WeSendEventButNoAnswer request = "<<request<<"\n";
    string cmd = "echo WeSendEventButNoAnswer request = "+request+">>"+logfile;
    system(cmd.c_str());
}

void WSSMonitor::Check()
{
    while(1)
    {
	boost::this_thread::sleep( boost::posix_time::milliseconds(10000));
	std::cout<<"Make check\n";
	
	vector<CDRReport> reports;
	db.getCDRReports(reports,"1");
	
	for(auto x=reports.begin();x!=reports.end();++x)
	{
	    CDRReport tmp = (*x);
	    std::cout<<"["<<tmp.origcallid<<"] ["<<tmp.callid<<"]  ["<<tmp.request<<"] ["<<tmp.responce<<"] ["<<tmp.uniqueid<<"] ["<<tmp.type<<"]\n";
	    if((tmp.request=="NULL")&&(tmp.callid!="NULL"))
		WeHaveCDRButNotSendEvent(tmp.origcallid,tmp.callid);
	    if((tmp.request!="NULL")&&(tmp.callid=="NULL"))
		WeHaveEventButNoCDR(tmp.request);
	    if((tmp.request!="NULL")&&(tmp.responce=="NULL"))
		WeSendEventButNoAnswer(tmp.request);
	    
	    if(tmp.responce=="NULL")
	    {
		if(tmp.type=="1")
		    cdr.sendJsonRequest(tmp.sendData,tmp.request);
		else if(tmp.type=="2")
		    crm.sendRequestAndStore(tmp.sendData,tmp.request);
	    }
	}
    }
}

void WSSMonitor::unstop()
{
    tgroup.join_all();
}