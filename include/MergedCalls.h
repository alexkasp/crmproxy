#ifndef MERGED_CALLS
#define MERGED_CALLS
#include <map>
#include <string>
#include <boost/thread/mutex.hpp>

using namespace std;

class MergedCalls
{
    boost::mutex Lock;
    
    map<string,string> callList;
    public:
    
    void addMergedCall(string newcallid,string callid);
    string getMergedCall(string callid);
    void eraseMergedCall(string callid);
    int getSize();
    void print();
    map<string,string>& getData();
    boost::mutex& getLock();    
};
#endif