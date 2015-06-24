#include "RegisterMonitor.h"

int RegisterMonitor::Execute(ParamMap& data)
{
    string number;
    string status;
    string address;
    
    if(parser->parsedata(data,number,status,address))
    {
	cout<<"START REGISTER EVENT"<<endl;
	
	auto it = UidToIdStorage.find(number.substr(0,6));
	if(it!=UidToIdStorage.end())
	{
	    DBWorker.PutRegisterEvent((*it).second,number,status);
	}
	else
	{
	    string uid;
	    string id;
	    if(DBWorker.getUid(UidToIdStorage,uid,id))
	    {
		DBWorker.PutRegisterEvent(id,number,status);
	    }
	}
    }
    return 0;
}