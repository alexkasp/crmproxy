#include <MergedCalls.h>

void MergedCalls::addMergedCall(string newcallid,string callid)
{
    callList[newcallid]=callid;
}

string MergedCalls::getMergedCall(string callid)
{
    auto c = callList.find(callid);
    if(c!=callList.end())
	return c->second;
    return callid;
}

void MergedCalls::eraseMergedCall(string callid)
{
    callList.erase(callid);
}