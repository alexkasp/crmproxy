#include <MergedCalls.h>
#include <iostream>


void MergedCalls::addMergedCall(string newcallid,string callid)
{
    boost::mutex::scoped_lock lock(Lock);
    callList[callid]=newcallid;
}

int MergedCalls::getSize()
{
    return callList.size();
}

boost::mutex&  MergedCalls::getLock()
{
    return Lock;                                                                                          
}

map<string,string>& MergedCalls::getData()
{
    return callList;
}

void MergedCalls::print()
{
    boost::mutex::scoped_lock lock(Lock);
    for(auto it=callList.begin();it!=callList.end();++it)
	std::cout<<"MergedCalls "<<(it->first)<<"  "<<(it->second)<<"\n";
}

string MergedCalls::getParentCall(string callid)
{
    boost::mutex::scoped_lock lock(Lock);
    for(auto x = callList.begin();x!=callList.end();++x)
    {
	if((x->second).compare(callid)==0)	
	    return x->first;
    }
    return "";
}

string MergedCalls::getMergedCall(string callid)
{
    boost::mutex::scoped_lock lock(Lock);
    auto c = callList.find(callid);
    if(c!=callList.end())
	return c->second;
    return callid;
}

void MergedCalls::eraseMergedCall(string callid)
{
    boost::mutex::scoped_lock lock(Lock);
    
    auto it = callList.find(callid);
    if(it!=callList.end())
	callList.erase(it);
}