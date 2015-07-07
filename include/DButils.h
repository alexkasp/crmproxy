
#ifdef __LINUX__
#include <string>
#include <map>
#include <mysql++.h>
using namespace std;


class DButils
{
	
	shared_ptr<mysqlpp::Connection> conn;
	const string delimiter = "=";
	const string vardelimiter = "$";
	
	const int VALUEPREFIXLENGTH = 1;
	const int VALUEPOSTFIXLENGTH = 3;
	
	string host,pass,login,db;
	
	int parse(string msg,string delimiter,string& param,string& value);
	void addUidToMap(map<string,string>& storage,mysqlpp::StoreQueryResult::const_iterator it);
	int parseParam(string msg,string& param,string& value);
    public:
	DButils();
	
	void PutRegisterEvent(string id,string number,string status);
	int getAuthParams(string filename);
	int connect();
	int getUidList(map<string,string>& uidToUserId);
	int getUid(map<string,string>& uidToUserId,string uid,string& id);
};
#else

#include <string>
#include <map>
using namespace std;


class DButils
{
    
   
    const string delimiter = "=";
    const string vardelimiter = "$";
    
    const int VALUEPREFIXLENGTH = 1;
    const int VALUEPOSTFIXLENGTH = 3;
    
    string host,pass,login,db;
    
    int parse(string msg,string delimiter,string& param,string& value);
    int parseParam(string msg,string& param,string& value);
public:
    DButils();
    
    void PutRegisterEvent(string id,string number,string status);
    int getAuthParams(string filename);
    int connect();
    int getUidList(map<string,string>& uidToUserId);
    int getUid(map<string,string>& uidToUserId,string uid,string& id);
};


#endif