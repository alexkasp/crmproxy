#include <map>
#include <string>

using namespace std;

class MergedCalls
{
    map<string,string> callList;
    public:
    void addMergedCall(string newcallid,string callid);
    string getMergedCall(string callid);
    void eraseMergedCall(string callid);
};