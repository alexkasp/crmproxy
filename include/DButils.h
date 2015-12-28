#ifndef DBUTILS_HEADER
#define DBUTILS_HEADER 1
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
	int getIncomeCallData(string uniqueid,string& operatorNum);
    
    public:
	DButils();
	
	void getCDR(string callid,map<string,string>& data);
	void putCDR(map<string,string>& data);
	int getCallData(string userId,string clientNum,string& operatorNum);
	int getCrmUsers(map<string,int>& users);
	void PutRegisterEvent(string id,string number,string status,string address);
	int getAuthParams(string filename);
	int connect();
	int getUidList(map<string,string>& uidToUserId);
	int getUid(map<string,string>& uidToUserId,string uid,string& id);
};


#endif