#include <DButils.h>
#include <fstream>
#include <iostream>

int DButils::getAuthParams(string filename)
{
    ifstream system_variables(filename);
    if(!system_variables)
	return 0;
    
    string  strbuf;
    	
    while(getline(system_variables,strbuf))
    {
	string param;
	string value;
	
	if(parseParam(strbuf,param,value))
	{
	    if(param == "ConnectStr")
	    {
		host = value;
	    }
	    else if(param == "ConnectPass")
	    {
		pass = value;
	    }
	    else if(param == "ConnectUser")
	    {
		login = value;
	    }
	    else if(param == "ConnectDB")
	    {
		db=value;
	    }
	    
	}
    }
    cout<<db<<" "<<host<<" "<<login<<" "<<pass<<endl;
    if((!host.empty())&&(!db.empty())&&(!pass.empty())&&(!login.empty()))
	return 1;

    return 0;

}

DButils::DButils()
{
#ifdef __LINUX__
    cout<<"try to create connection object"<<endl;
    conn = shared_ptr<mysqlpp::Connection>(new mysqlpp::Connection(false));
    cout<<"it is OK"<<endl;
#endif
}

int DButils::getUidList(map<string,string>& uidToUserId)
{
#ifdef __LINUX__
    mysqlpp::Query query = conn->query("select id,uid from UserConfig");
        
     if (mysqlpp::StoreQueryResult res = query.store()) 
     {
        for(auto it=res.begin();it!=res.end();++it)
        {
    	    addUidToMap(uidToUserId,it);
        }
         return 1;
     }
#endif
     return 0;
}

void DButils::PutRegisterEvent(string id,string number,string status)
{
#ifdef __LINUX__
     mysqlpp::Query query = conn->query("insert into RegisterEvents(eventtime,number,status,uid) values(Now(),'%0','%1',%2)");
     query.parse();
     mysqlpp::SQLQueryParms parms;
     
     
    parms.push_back( mysqlpp::sql_varchar(number) ); 
    parms.push_back( mysqlpp::sql_varchar(status) ); 
    parms.push_back( mysqlpp::sql_varchar( id ) );
    
    if(!query.execute( parms ))
    {
	cerr << "DB connection failed: " << conn->error() << endl;
	                exit(0);
	                
    }
    else
	cout <<"PUTREGISTER EVENT"<<endl;
#endif
}

#ifdef __LINUX__
void DButils::addUidToMap(map<string,string>& storage,mysqlpp::StoreQueryResult::const_iterator it)
{
    mysqlpp::Row row = *it;
    string uid = string(row[1].data(),row[1].length());
    string id = string(row[0].data(),row[0].length());
    storage[uid]=id;
}

#endif

int DButils::getUid(map<string,string>& uidToUserId,string uid,string& id)
{
#ifdef __LINUX__
    mysqlpp::Query query = conn->query("select id,uid from UserConfig where uid="+uid);
        
     if (mysqlpp::StoreQueryResult res = query.store()) 
     {
        for(auto it=res.begin();it!=res.end();++it)
        {
    	    
    	    addUidToMap(uidToUserId,it);
    	    mysqlpp::Row row = *it;
    	    id = string(row[0].data(),row[0].length());
    	    
        }
         return 1;
     }
#endif
     return 0;
}

int DButils::connect()
{
#ifdef __LINUX__
     if (conn->connect(db.c_str(),host.c_str(),login.c_str(),pass.c_str()))
     {
        return 1;
     }
     else{
        cerr << "DB connection failed: " << conn->error() << endl;
                return 0;
     }
    
#endif
    return 0;
}

int DButils::parse(string msg,string delimiter,string& param,string& value)
{
    size_t pos = 0;
    if((pos = msg.find(delimiter))!= std::string::npos)
    {
	param = msg.substr(0, pos);
	msg.erase(0, pos + delimiter.length());
	value=msg;
	return 1;	
		
    }
    
    return 0;
}

int DButils::parseParam(string msg,string& param,string& value)
{
    if(parse(msg,delimiter,param,value))
    {
	string tmpparam;
	if((parse(param,vardelimiter, tmpparam,param))&&(value.length()>3))
	{
	    value = value.substr(VALUEPREFIXLENGTH,value.length()-VALUEPOSTFIXLENGTH);
	    return 1;
	}
    }
    
    return 0;
}