#include <DButils.h>
#include <fstream>
#include <iostream>
#include <exception>      // std::exception

string DButils::getServerId()
{
    return serverid;
}

int DButils::registerNode(string callid,string time, string treeid, string answernum)
{
  boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
  if(!Lock)
  {
      cout<<"ERROR GET DB LOCK in dbutils PutRegisterevent\n";
      return 0;
  }
  mysqlpp::Query query = conn->query("insert into CallRun(nodeid,nodetype,uniqueid,time,treeid,answernum) values(0,100,'%0',UNIX_TIMESTAMP(Now())-'%1',%2,'%3')");
  query.parse();
 
  mysqlpp::SQLQueryParms params;
  params.push_back( mysqlpp::sql_varchar(callid) );
  params.push_back( mysqlpp::sql_varchar(time) );
  params.push_back( mysqlpp::sql_varchar( treeid ) );
  params.push_back( mysqlpp::sql_varchar(answernum) );

  if(!query.execute( params ))
  {
     cout << "DB connection failed: " << conn->error()<< query.str() << "\n" << endl;
  }
                                                                                 
}

int DButils::updateNode(string callid,string curnode,string answernum)
{
  boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
  if(!Lock)
  {
      cout<<"ERROR GET DB LOCK in dbutils PutRegisterevent\n";
      return 0;
  }
  mysqlpp::Query query = conn->query("update CallRun set answernum = '%0' where uniqueid = '%1' and nodeid = %2 and nodetype = 100 order by id DESC limit 1;");
  query.parse();
 
  mysqlpp::SQLQueryParms params;
  params.push_back( mysqlpp::sql_varchar(answernum) );
  params.push_back( mysqlpp::sql_varchar(callid) );
  params.push_back( mysqlpp::sql_varchar(curnode) );

  if(!query.execute( params ))
  {
     cout << "DB connection failed: " << conn->error()<< query.str() << "\n" << endl;
  }
                                                                                 
}

string DButils::getPBXServerId()
{
    return pbxServerid;
}

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
	    else if(param == "extregServerID")
	    {	
		std::cout<<"We found serverID\n";
		serverid = value;
	    }
	    else if(param == "ServerID")
	    {	
		std::cout<<"We found ATS serverId\n";
		pbxServerid = value;
	    }
	    
	}
    }
    cout<<"db " <<db<<" host "<<host<<" login( "<<getUserParamName()<<" )"<<login<<" pass "<<pass<<" serverId "<<serverid<<endl;
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
    cout<<"try to create redis connection object"<<endl;
    redis.connect("127.0.0.1", 6379, [](const std::string& host, std::size_t port, cpp_redis::connect_state status) {
        if (status == cpp_redis::connect_state::dropped) {
              std::cout << "client disconnected from " << host << ":" << port << std::endl;
                  }
                    });
}

DButils::~DButils()
{
    
}

int DButils::getUidList(map<string,string>& uidToUserId)
{
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getUidList\n";
        return 0;
    }
    
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

void DButils::addSendEventReportEntry(string callid,string request,string ats,string userid,string type,string sendData)
{
    return;
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils addSendEventReportEntry\n";
        return;
    }
    
    if(callid.empty())
	callid="empty";
    if(request.empty())
	request= "empty";
    if(ats.empty())
	ats = "0";
    if(userid.empty())
	userid="0";
    if(sendData.empty())
	sendData = "empty";
    if(type.empty())
	type = "100";
	
    try
    {	
    
    
    }
     catch (std::exception& e)
       {
           std::cout << "exception caught: " << e.what() << '\n';
           std::cout << "callid = " << callid << " request = "<< request << " ats = " << ats << " userid = " << userid << " type = "<<type<<"\n send data \n"<<sendData<<"\n";
             }
    
    return;
}

void DButils::redisCommit()
{
   redis.sync_commit();
}

void DButils::setRedisVariable(string setname, string varname, string value)
{
redis.send({"HSET", setname, varname, value}, [](cpp_redis::reply& reply) {
    std::cout << "hset: " << reply << std::endl;
        // if (reply.is_string())
            //   do_something_with_string(reply.as_string())
              });
}

void DButils::completeEventReportEntry(string callid,string responce,string answerData)
{
    return;
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils completeEventReportEntry\n";
        return;
    }
     if(responce.empty())
        responce="empty";
    if(answerData.empty())
	answerData = "empty";

     
    try
    { 
    
    }
    catch (std::exception& e)
    {
       std::cout << "exception caught in completeEventReportEntry: " << e.what() << '\n';
       std::cout << " callid = "<< callid << "responce " << responce <<"\n"<<answerData<<"\n";
    }                             
    return;
}

void DButils::getCDRReports(vector<CDRReport>& reports,string period)
{
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getCDRReport\n";
        return;
    }

    try{
	         
	    CDRReport report;
	    
	                                                                                      
	    
    
    }
    catch (std::exception& e)
    {
       std::cout<<"ERROR EXCEPTION in getCDRReports "<< e.what()<<"\n";
    }
}

void DButils::getCDR(string uniqueid,map<string,string>& data)
{
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getCDR\n";
        return;
    }
    mysqlpp::Query query = conn->query();
    
//    query << "select label,raiting,newstatus,crmcall from additionaldata where uniqueid='"<<uniqueid<<"'";
//    std::cout<<query.str();
    
    try
    {
/*	if (mysqlpp::StoreQueryResult res = query.store()) 
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
*/     
        query << "select isblock from records where callid='"<<uniqueid<<"'";
    
	if (mysqlpp::StoreQueryResult res = query.store()) 
        {
    	    for(auto it=res.begin();it!=res.end();++it)
    	    {
    	    
    		mysqlpp::Row row = *it;
		data["isblock"]=row[0].data();
	    }
	}
    }
    catch (std::exception& e)
    {
       std::cout<<"ERROR EXCEPTION in getCDR["<<uniqueid<< "] "<< e.what()<<"\n";
    }		 	 
     return;
}
void DButils::putCDR(map<string,string>& data)
{
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils putCDR\n";
        return;
    }
    
    mysqlpp::Query query = conn->query();
    
    string treeId = data["TreeId"];
    if(treeId.empty())
	treeId="0";
    
    string dst = data["dst_num"];
    if(dst.empty())
    {
    	dst = data["destination"];
    }
    
    string status = data["status"];
    if(status=="NOANSWER")
	status = "NO ANSWER";
    
    string recordName = data["recordname"];	
    int numRecords = !(recordName.empty());
    query<<"insert into callstat(calldate,src,dst,duration,billsec,disposition,uniqueid,numrecords,recordName,numnodes,answertime,treeId,from2,to2,userId,directProcess) values(Now(),"<<
    "'"<<data["src_num"]<<"','"<<dst<<"',"<<data["duration"]<<","<<data["billableseconds"]<<",'"<<status<<"','"<<data["call_id"]<<"',"<<numRecords<<",'"<<data["recordname"]<<"',6"<<",0,"<<treeId<<",'"<<data["src_num"]<<"','"<<dst<<"',"<<data["userId"]<<",1)";
    
    
    query.execute();
     if (mysqlpp::StoreQueryResult res = query.store()) 
         {
                  }
                  
                      else 
                          {
                                  std::cout << "Failed to get item list: " << query.error() << endl;
                                              }
}


int DButils::getCallData(string userId,string clientNum,string& operatorNum)
{
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getCallDAta\n";
        return 0;
    }
    map<string,map<string,string>> callsWithDate;
    

    
    stringstream queryStr;
    string clientNumSub;
    if(clientNum.length()>9)
	clientNumSub = clientNum.substr(2,clientNum.length());
    else
	clientNumSub = clientNum;
    
    queryStr << "select froma,uniqueid, IF(froma like '%"<<clientNum<<"','in','out' ) from cdr where accountcode='"<<userId<<"' and (dst like '%"<<clientNumSub<<"' or froma like '%"<<clientNum<<"') and calldate > DATE_SUB(CURDATE(), INTERVAL 3 month) and accountcode="<<userId<<
    "  and disposition = 'ANSWERED' order by calldate DESC limit 100;";
    
    string queryStringPrepared = queryStr.str();
    
    
    mysqlpp::Query query = conn->query(queryStringPrepared);
        
     if (mysqlpp::StoreQueryResult res = query.store()) 
     {
        for(auto it=res.begin();it!=res.end();++it)
        {
    	    
    	    mysqlpp::Row row = *it;
    	    
    	    if(strcmp(row[2].data(),"out")==0)
    	    {
    		operatorNum = row[0].data();
    		return 1;
    	    }
    	    else
    	    {
    		if(getIncomeCallData(row[1].data(),userId,operatorNum))
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

int DButils::getIncomeCallData(string uniqueid,string userId,string& operatorNum)
{

/*    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getIncomeCalldata\n";
        exit(-1);
    }*/
    stringstream ss;
    ss<<"select answernum from CallRun where uniqueid='"<<uniqueid<<"' and ( nodetype=0 and 100 in (select nodetype from callrun where uniqueid = '"<<uniqueid<<"') or nodetype=111) order by time DESC";
    
    
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
         return 0;
     }
     
     return 0;


}
int DButils::getTestResult(string testid,string callid,vector<TestResult>& result,vector<TestTemplate>& etalon)
{
    
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getCRMUsers\n";
        return 0;
    }
    string querystr = "select time,nodetype from CallRun where uniqueid = '"+callid+"'";
    mysqlpp::Query query = conn->query(querystr);
     
    try{ 
        if (mysqlpp::StoreQueryResult res = query.store()) 
        {
    	    for(auto it=res.begin();it!=res.end();++it)
    	    {
    		mysqlpp::Row row = *it;
    		TestResult tr;
    	    
    	    
    		tr.time = row[0];
    		tr.nodetype = row[1];
    	    
    		result.push_back(tr);
    	    
    	    
    	    }
        }
        else
        {
    	    cout << "DB connection failed: " << conn->error()<< "["<<query.str() << "]\n" << endl;
    	    return 0;
        }
     }
     catch (std::exception& e)
    {
       std::cout << "exception caught in getTestResult(get results): " << e.what() << '\n';
       return 0;
    }
     
     querystr = "select time,nodetype,timediffallow,checkaction from testcheck where testid = "+testid;
     mysqlpp::Query queryResult = conn->query(querystr);
     try
     {
        if (mysqlpp::StoreQueryResult res = queryResult.store()) 
        {
    	    for(auto it=res.begin();it!=res.end();++it)
    	    {
    		mysqlpp::Row row = *it;
    		TestTemplate tt;
    	    
    		tt.time = row[0];
    		tt.nodetype = row[1];
    		tt.timediffallow = row[2];
    		tt.checkaction = row[3];
    	    
    		etalon.push_back(tt);
    	    
    	    }
        }
        else
        {
    	    cout << "DB connection failed: " << conn->error()<< "["<<queryResult.str() << "]\n" << endl;
    	    return 0;
        }
     }
     catch (std::exception& e)
    {
       std::cout << "exception caught in getTestREsult(get etalon): " << e.what() << '\n';
       return 0;
    }
     return 1;

}

int DButils::getTestById(string testid,string& from,string& to)
{
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getCRMUsers\n";
        return 0;
    }
    string querystr = "select treeA,treeB from testlist where id = "+testid;//+testid;
    mysqlpp::Query query = conn->query(querystr);
     
     if (mysqlpp::StoreQueryResult res = query.store()) 
     {
        for(auto it=res.begin();it!=res.end();++it)
        {
    	    mysqlpp::Row row = *it;
    	    from = row[0].data();
    	    to = row[0].data();
    	    
        }
         return 1;
     }
     else
        cout << "DB connection failed: " << conn->error()<< query.str() << "\n" << endl;
     return 0;

}

int DButils::getCrmUsers(map<string,int>& users)
{
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getCRMUsers\n";
        return 0;
    }
    mysqlpp::Query query = conn->query("select id from UserConfig where usecrm=1");
        
     if (mysqlpp::StoreQueryResult res = query.store()) 
     {
        for(auto it=res.begin();it!=res.end();++it)
        {
    	    mysqlpp::Row row = *it;
    	    users[row[0].data()]=1;
    	    
        }
         return 1;
     }
     return 0;

}

void DButils::PutRegisterEvent(string id,string number,string status,string address)
{
    return;
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils PutRegisterevent\n";
        return;
    }
     mysqlpp::Query query = conn->query("insert into RegisterEvents(eventtime,number,status,uid,address) values(Now(),'%0','%1',%2,'%3')");
     query.parse();
     mysqlpp::SQLQueryParms parms;
     
     
    parms.push_back( mysqlpp::sql_varchar(number) ); 
    parms.push_back( mysqlpp::sql_varchar(status) ); 
    parms.push_back( mysqlpp::sql_varchar( id ) );
    parms.push_back( mysqlpp::sql_varchar(address) ); 
    
    if(!query.execute( parms ))
    {
	cout << "DB connection failed: " << conn->error()<< query.str() << "\n" << endl;
		            //    exit(0);
	                
    }
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

    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getUid\n";
        return 0;
    }
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
	    try{
	query.execute();
    }
     catch (std::exception& e)
       {
           std::cout << "exception caught: " << e.what() << '\n';
             }

        return 1;
     }
     else{
        cout << "DB connection failed: " << conn->error() << endl;
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
        if((parse(param,vardelimiter, tmpparam,param))/*&&(value.length()>3)*/)
        {
    	    if(value[0] == '\'')
    		value = value.substr(VALUEPREFIXLENGTH,value.length()-VALUEPOSTFIXLENGTH);
    	    else
    		value = value.substr(0,value.length()-INTVALUEPOSTFIXLENGTH);	
            return 1;
        }
    }
    
    return 0;
}