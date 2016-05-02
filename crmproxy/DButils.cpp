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
	
	std::cout<<strbuf<<"\n";
	
	if(parseParam(strbuf,param,value))
	{
	    if(param == getHostParamName())
	    {
		
		host = value;
	    }
	    else if(param == getPassParamName())
	    {
		pass = value;
	    }
	    else if(param == getUserParamName())
	    {
		login = value;
	    }
	    else if(param == getDBParamName())
	    {
		db=value;
	    }
	    
	}
    }
    cout<<"db " <<db<<" host "<<host<<" login( "<<getUserParamName()<<" )"<<login<<" pass "<<pass<<endl;
    if((!host.empty())&&(!db.empty())&&(!pass.empty())&&(!login.empty()))
	return 1;

    return 0;

}

string DButils::getHostParamName()
{
    return "ConnectStr";
}

string DButils::getPassParamName()
{
    return "ConnectPass";
}

string DButils::getUserParamName()
{
    return "ConnectUser";
}

string DButils::getDBParamName()
{
    return "ConnectDB";
}


DButils::DButils()
{
    cout<<"try to create connection object"<<endl;
    conn = shared_ptr<mysqlpp::Connection>(new mysqlpp::Connection(false));
    
    cout<<"it is OK"<<endl;
}

int DButils::getUidList(map<string,string>& uidToUserId)
{
    boost::mutex::scoped_lock Lock(dblock);
    
    mysqlpp::Query query = conn->query("select id,uid from UserConfig");
        
     if (mysqlpp::StoreQueryResult res = query.store()) 
     {
        for(auto it=res.begin();it!=res.end();++it)
        {
    	    addUidToMap(uidToUserId,it);
        }
         return 1;
     }
     return 0;
}

void DButils::getCDR(string uniqueid,map<string,string>& data)
{
    boost::mutex::scoped_lock Lock(dblock);
    
    mysqlpp::Query query = conn->query();
    
    query << "select label,raiting,newstatus,crmcall from additionaldata where uniqueid='"<<uniqueid<<"'";
    std::cout<<query.str();
    
    if (mysqlpp::StoreQueryResult res = query.store()) 
     {
        for(auto it=res.begin();it!=res.end();++it)
        {
    	    
    	    mysqlpp::Row row = *it;
    	    std::cout<<"CDR DATA "<<row[0].data()<<"//"<<row[1].data()<<"//"<<row[2].data()<<"\n";
	    data["label"]=row[0].data();
	    data["raiting"] = row[1].data();
	    data["newstatus"] = row[2].data();
	    data["crmcall"] = row[3].data();
	}
    }	 
     
     query << "select isblock from records where callid='"<<uniqueid<<"'";
    std::cout<<query.str();
    
    if (mysqlpp::StoreQueryResult res = query.store()) 
     {
        for(auto it=res.begin();it!=res.end();++it)
        {
    	    
    	    mysqlpp::Row row = *it;
	    data["isblock"]=row[0].data();
	}
    }  
     return;
}
void DButils::putCDR(map<string,string>& data)
{
    boost::mutex::scoped_lock Lock(dblock);
    
    std::cout<<"make query\n";
    mysqlpp::Query query = conn->query();
    
    string treeId = data["TreeId"];
    if(treeId.empty())
	treeId="0";
    
    string dst = data["dst_num"];
    if(dst.empty())
    {
	std::cout<<"dst empty\n";
    	dst = data["destination"];
    }
    
    string status = data["status"];
    if(status=="NOANSWER")
	status = "NO ANSWER";
    
    string recordName = data["recordname"];	
    int numRecords = !(recordName.empty());
    query<<"insert into callstat(calldate,src,dst,duration,billsec,disposition,uniqueid,numrecords,recordName,numnodes,answertime,treeId,from2,to2,userId,directProcess) values(Now(),"<<
    "'"<<data["src_num"]<<"','"<<dst<<"',"<<data["duration"]<<","<<data["billableseconds"]<<",'"<<status<<"','"<<data["call_id"]<<"',"<<numRecords<<",'"<<data["recordname"]<<"',6"<<",0,"<<treeId<<",'"<<data["src_num"]<<"','"<<dst<<"',"<<data["userId"]<<",1)";
    
    std::cout<<query.str()<<"\n";
    
    query.execute();
     if (mysqlpp::StoreQueryResult res = query.store()) 
         {
              std::cout<<"success query\n";
                  }
                  
                      else 
                          {
                                  std::cout << "Failed to get item list: " << query.error() << endl;
                                              }
}


int DButils::getCallData(string userId,string clientNum,string& operatorNum)
{
    boost::mutex::scoped_lock Lock(dblock);
    
    map<string,map<string,string>> callsWithDate;
    

    
    stringstream queryStr;
    string clientNumSub;
    if(clientNum.length()>9)
	clientNumSub = clientNum.substr(2,clientNum.length());
    
    queryStr << "select froma,uniqueid, IF(froma like '%"<<clientNum<<"','in','out' ) from cdr where accountcode='"<<userId<<"' and (dst like '%"<<clientNumSub<<"' or froma like '%"<<clientNum<<"') and calldate > DATE_SUB(CURDATE(), INTERVAL 3 month) and accountcode="<<userId<<
    "  and disposition = 'ANSWERED' order by calldate DESC limit 100;";
    
    string queryStringPrepared = queryStr.str();
    
    std::cout<<"getCallData v 1.1 "<<queryStringPrepared<<"\n";
    
    mysqlpp::Query query = conn->query(queryStringPrepared);
        
     if (mysqlpp::StoreQueryResult res = query.store()) 
     {
        for(auto it=res.begin();it!=res.end();++it)
        {
    	    
    	    mysqlpp::Row row = *it;
    	    
    	    std::cout<<row[0].data()<<"//"<<row[1].data()<<"//"<<row[2].data()<<"\n";
    	    if(strcmp(row[2].data(),"out")==0)
    	    {
    		operatorNum = row[0].data();
    		return 1;
    	    }
    	    else
    	    {
    		if(getIncomeCallData(row[1].data(),operatorNum) > 0)
    		{
    		    return 1;
    		}
    	    }
        }
         return -1;
     }
     else 
     {
          std::cout << "Failed to get item list: " << query.error() << endl;
     }
    return 0;

}

int DButils::getIncomeCallData(string uniqueid,string& operatorNum)
{

    stringstream ss;
    ss<<"select answernum from CallRun where uniqueid='"<<uniqueid<<"' and nodetype=0 order by time DESC";
    
    
    std::cout<<" getIncomeCallData "<<(ss.str())<<"\n";
    mysqlpp::Query query = conn->query(ss.str());
        
     if (mysqlpp::StoreQueryResult res = query.store()) 
     {
        for(auto it=res.begin();it!=res.end();++it)
        {
    	    mysqlpp::Row row = *it;
    	    if(!(row[0]).empty())
    	    {
    		operatorNum = row[0].data();
    		return 1;
    	    }
        }
         return -1;
     }
     return 0;


}

int DButils::getCrmUsers(map<string,int>& users)
{
    boost::mutex::scoped_lock Lock(dblock);
    
    std::cout<<"getCrmUsers"<<std::endl;
    mysqlpp::Query query = conn->query("select id from UserConfig where usecrm=1");
        
     if (mysqlpp::StoreQueryResult res = query.store()) 
     {
        for(auto it=res.begin();it!=res.end();++it)
        {
    	    mysqlpp::Row row = *it;
    	    users[row[0].data()]=1;
    	    
    	    std::cout<<"getCrmUsers "<<row[0].data()<<" = 1"<<std::endl;
        }
         return 1;
     }
     return 0;

}

void DButils::PutRegisterEvent(string id,string number,string status,string address)
{
     mysqlpp::Query query = conn->query("insert into RegisterEvents(eventtime,number,status,uid,address) values(Now(),'%0','%1',%2,'%3')");
     query.parse();
     mysqlpp::SQLQueryParms parms;
     
     
    parms.push_back( mysqlpp::sql_varchar(number) ); 
    parms.push_back( mysqlpp::sql_varchar(status) ); 
    parms.push_back( mysqlpp::sql_varchar( id ) );
    parms.push_back( mysqlpp::sql_varchar(address) ); 
    
    if(!query.execute( parms ))
    {
	cerr << "DB connection failed: " << conn->error()<< query.str() << "\n" << endl;
		            //    exit(0);
	                
    }
    else
	cout <<"PUTREGISTER EVENT"<<endl;
}

void DButils::addUidToMap(map<string,string>& storage,mysqlpp::StoreQueryResult::const_iterator it)
{
    
    mysqlpp::Row row = *it;
    string uid = string(row[1].data(),row[1].length());
    string id = string(row[0].data(),row[0].length());
    storage[uid]=id;
}


int DButils::getUid(map<string,string>& uidToUserId,string uid,string& id)
{
    boost::mutex::scoped_lock Lock(dblock);
    
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
     return 0;
}

int DButils::connect()
{
    conn->set_option(new mysqlpp::ReconnectOption(true));
     if (conn->connect(db.c_str(),host.c_str(),login.c_str(),pass.c_str()))
     {
        mysqlpp::Query query = conn->query("set names utf8");
	query.execute();
        return 1;
     }
     else{
        cerr << "DB connection failed: " << conn->error() << endl;
                return 0;
     }
    
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