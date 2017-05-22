#include <DButils.h>

class gatewayData
{
    public:
	string userId;
	string uid;
	string type;
	string login;
	string servid;
	string gatewayname;
	string status;
	int maxtrycount;
	int buntime;

};

class UtilDButils: public DButils
{
    protected:
	virtual string getHostParamName();
	virtual string getPassParamName();
        virtual string getUserParamName();
        virtual string getDBParamName();
	            
    public:
	int loadGateways(map<string,gatewayData>& gateways,std::string extregid);
	int updateStaticProvs(string userId,string login,string status);
};