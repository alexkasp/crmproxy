#ifndef DBUTILS_HEADER
#define DBUTILS_HEADER 1

#include <boost/thread/mutex.hpp>
#include <string>
#include <map>
#include <vector>
#include <mysql++.h>
using namespace std;

class CDRReport
{
    public:
	string callid;
	string origcallid;
	string responce;
	string request;
	string uniqueid;
	string sendData;
	string type;
};

class TestResult
{
    public:
    int time;
    int nodetype;
};

class TestTemplate
{
    public:
    int time;
    int nodetype;
    int timediffallow;
    int checkaction;
};

class DButils
{
	
	
	const string delimiter = "=";
	const string vardelimiter = "$";
	
	const int VALUEPREFIXLENGTH = 1;
	const int VALUEPOSTFIXLENGTH = 3;
	
	string host,pass,login,db;
	
	int parse(string msg,string delimiter,string& param,string& value);
	void addUidToMap(map<string,string>& storage,mysqlpp::StoreQueryResult::const_iterator it);
	int parseParam(string msg,string& param,string& value);
	int getIncomeCallData(string uniqueid,string& operatorNum);
	
    
	
    protected:
	boost::timed_mutex dblock;
	
	virtual string getHostParamName();
        virtual string getPassParamName();
        virtual string getUserParamName();
        virtual string getDBParamName();
	            
	shared_ptr<mysqlpp::Connection> conn;
    public:
	DButils();
	int getTestResult(string testid,string callid,vector<TestResult>& result,vector<TestTemplate>& etalon);
	int getTestById(string testid,string& from,string& to);
	void addSendEventReportEntry(string callid,string request,string ats,string userid,string type,string sendData);
	void completeEventReportEntry(string request,string responce,string answerData);
	
	void getCDR(string callid,map<string,string>& data);
	void putCDR(map<string,string>& data);
	int getCallData(string userId,string clientNum,string& operatorNum);
	int getCrmUsers(map<string,int>& users);
	void PutRegisterEvent(string id,string number,string status,string address);
	int getAuthParams(string filename);
	int connect();
	int getUidList(map<string,string>& uidToUserId);
	int getUid(map<string,string>& uidToUserId,string uid,string& id);
	
	void getCDRReports(vector<CDRReport>& reports,string period);
	
};


#endif