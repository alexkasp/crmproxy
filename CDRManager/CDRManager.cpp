#include <CDRManager.h>
#include <iostream>

CDRManager::CDRManager(LoggerModule& _lm,DButils& db):lm(_lm),DBWorker(db)
{

}

void CDRManager::processCDR(map<string,string>& data)
{
    std::cout<<"process CDR\n";
    
    for(auto x=data.begin();x!=data.end();++x)
	std::cout<<(x->first)<<" : "<<(x->second)<<"\n";
    
    if(data["event"]=="2")
    {
	std::cout<<"CDRManager::processCDR -> DBWorker.putCDR\n";
	DBWorker.putCDR(data);
    }
    else
	std::cout<<"not 2 event\n";
}