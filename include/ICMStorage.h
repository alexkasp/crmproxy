#ifndef ICM_STORAGE
#define ICM_STORAGE 1

#include <map>
#include <string>
using namespace std;


struct cdrrecord
{
    string callid;
    string calldate;
    string userid;
    string clientNum;
    string operatorNum;

    cdrrecord(string _calldate,string _operatorName,string _callid)
    {
        calldate = _calldate;
        operatorNum = _operatorName;
        callid = _callid;
    }
};

typedef cdrrecord CDRData;

class ICMStorage
{
    int limit;
    map<string,map<string,CDRData>> aNumToCDRData;
public:
    int getCDRData(string userId,string aNum,CDRData& data);
    int putCDRData(string calldate,string userId,string aNum,string operatorNum,string callid);
    int putCDRData(map<string,string> rawData);
    
    ICMStorage(int limit);
};


#endif