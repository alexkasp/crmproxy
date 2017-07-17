#include <DButils.h>
#include <fstream>
#include <iostream>
#include <exception>      // std::exception

string DButils::getServerId()
{
    return serverid;
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
//		std::cout<<"get host "<<host<<"\n";
	    }
	    else if(param == getPassParamName())
	    {
		pass = value;
//		std::cout<<"get pass "<<pass<<"\n";
	    }
	    else if(param == getUserParamName())
	    {
		login = value;
//		std::cout<<"get login "<<login<<"\n";
	    }
	    else if(param == getDBParamName())
	    {
		db=value;
//		std::cout<<"get db "<<db<<"\n";
	    }
	    else if(param == "extregServerID")
	    {	
		std::cout<<"We found serverID\n";
		serverid = value;
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
    unsigned int j;
    serverid= "0";
    
    redisReply *reply;
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    redis = redisConnectWithTimeout("127.0.0.1", 6379, timeout);
    if (redis == NULL || redis->err)
    {
	if (redis)
	{
    	    printf("Connection error: %s\n", redis->errstr);
	    redisFree(redis);
	}
        else 
	{
	    printf("Connection error: can't allocate redis context\n");
	}
    }
}

DButils::~DButils()
{
    
    redisFree(redis);
}

int DButils::getUidList(map<string,string>& uidToUserId)
{
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getUidList\n";
        return 0;
    }
    cout<<"LOCK ACCEPTED\n";
    
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
    cout<<"LOCK ACCEPTED\n";
    std::cout<<"START addSendEventReportEntry\n";
    std::cout<<"addSendEventReportEntry\n";
    
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
    
     redisReply *reply = static_cast<redisReply*>(redisCommand(redis,"sadd calls %s", callid.c_str()));
     printf("SET: %s\n", reply->str);
     freeReplyObject(reply);
	 
     reply = static_cast<redisReply*>(redisCommand(redis,"HSET crmreport:%s %s %s", callid.c_str(), "request",request.c_str()));
     printf("SET: %s\n", reply->str);
     freeReplyObject(reply);
        
     reply = static_cast<redisReply*>(redisCommand(redis,"HSET crmreport:%s %s %s", callid.c_str(), "userid",userid.c_str()));
     printf("SET: %s\n", reply->str);
     freeReplyObject(reply);
    
     reply = static_cast<redisReply*>(redisCommand(redis,"HSET crmreport:%s %s %s", callid.c_str(), "type",type.c_str()));
     printf("SET: %s\n", reply->str);
     freeReplyObject(reply);
    
     reply = static_cast<redisReply*>(redisCommand(redis,"HSET crmreport:%s %s %s", callid.c_str(), "sendData",sendData.c_str()));
     printf("SET: %s\n", reply->str);
     freeReplyObject(reply);         
    
    }
     catch (std::exception& e)
       {
           std::cout << "exception caught: " << e.what() << '\n';
           std::cout << "callid = " << callid << " request = "<< request << " ats = " << ats << " userid = " << userid << " type = "<<type<<"\n send data \n"<<sendData<<"\n";
             }
    
    cout<<"END "<<callid<<"  addSendEventReportEntry\n";         
    return;
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
    cout<<"LOCK ACCEPTED\n";
     if(responce.empty())
        responce="empty";
    if(answerData.empty())
	answerData = "empty";

     std::cout<<"completeEventReportEntry\n";
     
    try
    { 
        redisReply *reply = static_cast<redisReply*>(redisCommand(redis,"HSET crmreport:%s %s %s", callid.c_str(), "responce",responce.c_str()));
        printf("SET: %s\n", reply->str);
        freeReplyObject(reply);
        
	reply = static_cast<redisReply*>(redisCommand(redis,"HSET crmreport:%s %s %s", callid.c_str(), "answerData",answerData.c_str()));
	printf("SET: %s\n", reply->str);
        freeReplyObject(reply);
    
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
    cout<<"LOCK ACCEPTED\n";

    try{
	         
	    CDRReport report;
	    
	    redisReply* Callsreply = static_cast<redisReply*>(redisCommand(redis,"smembers calls"));
	    
	    for (int j = 0; j < Callsreply->elements; j++)
	    {
	        printf("%u) %s\n", j, Callsreply->element[j]->str);
	     
		redisReply *reply = static_cast<redisReply*>(redisCommand(redis,"HGET crmreport:%s callid",Callsreply->element[j]->str));
		printf(": %s\n", reply->str);
		report.origcallid=reply->str;
		report.callid = reply->str;
	    
		reply = static_cast<redisReply*>(redisCommand(redis,"HGET crmreport:%s responce",Callsreply->element[j]->str));
		printf(": %s\n", reply->str);
		report.responce = reply->str;;
	    
		reply = static_cast<redisReply*>(redisCommand(redis,"HGET crmreport:%s request",Callsreply->element[j]->str));
		printf(": %s\n", reply->str);
		report.request = reply->str;;
	    
	//    redisReply *reply = static_cast<redisReply*>(redisCommand(redis,"HGET crmreport:%s request",Callsreply->element[j]->str));
	//    printf(": %s\n", reply->string);
	//    report.uniqueid=row[4].data();
	    
		reply = static_cast<redisReply*>(redisCommand(redis,"HGET crmreport:%s sendData",Callsreply->element[j]->str));
		printf(": %s\n", reply->str);
		report.sendData=reply->str;;
	    
		reply = static_cast<redisReply*>(redisCommand(redis,"HGET crmreport:%s type",Callsreply->element[j]->str));
		printf(": %s\n", reply->str);
		report.type = reply->str;;
    
		reports.push_back(report);
	     
	     
	        freeReplyObject(reply);
	    }
	     
	    freeReplyObject(Callsreply);
	                                                                                      
	    
    
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
    cout<<"LOCK ACCEPTED\n";    
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
	std::cout<<query.str();
    
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
    cout<<"LOCK ACCEPTED\n";
    
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
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getCallDAta\n";
        return 0;
    }
    cout<<"LOCK ACCEPTED\n";    
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
    		if(getIncomeCallData(row[1].data(),userId,operatorNum) > 0)
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
    cout<<"LOCK ACCEPTED\n";
    stringstream ss;
    ss<<"select answernum from CallRun where uniqueid='"<<uniqueid<<"' and nodetype=0 and answernum in (select accountcode from sipclientstable where userid="<<userId<<" and isprov=0) order by time DESC";
    
    
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
int DButils::getTestResult(string testid,string callid,vector<TestResult>& result,vector<TestTemplate>& etalon)
{
    
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getCRMUsers\n";
        return 0;
    }
    cout<<"LOCK ACCEPTED getTestResult v.1\n";
    string querystr = "select time,nodetype from CallRun where uniqueid = '"+callid+"'";
    mysqlpp::Query query = conn->query(querystr);
    cout <<querystr<<"\n"<< query<<"\n";
     
    try{ 
        if (mysqlpp::StoreQueryResult res = query.store()) 
        {
    	    for(auto it=res.begin();it!=res.end();++it)
    	    {
    		mysqlpp::Row row = *it;
    		TestResult tr;
    	    
    		std::cout<<"getTestResult "<<row[0].data()<<"  "<<row[1].data()<<std::endl;
    	    
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
     cout <<querystr<<"\n"<< queryResult<<"\n";
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
    	    
    		std::cout<<"getTestEtalon "<<row[0].data()<<"  "<<row[1].data()<<std::endl;
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
    cout<<"LOCK ACCEPTED getTestById v.1\n";
    string querystr = "select treeA,treeB from testlist where id = "+testid;//+testid;
    mysqlpp::Query query = conn->query(querystr);
     cout <<querystr<<"\n"<< query<<"\n";
     
     if (mysqlpp::StoreQueryResult res = query.store()) 
     {
        for(auto it=res.begin();it!=res.end();++it)
        {
    	    mysqlpp::Row row = *it;
    	    from = row[0].data();
    	    to = row[0].data();
    	    
    	    std::cout<<"getTestById "<<row[0].data()<<"  "<<row[1].data()<<std::endl;
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
    cout<<"LOCK ACCEPTED\n";    
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
    return;
    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils PutRegisterevent\n";
        return;
    }
    cout<<"LOCK ACCEPTED\n";
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

    boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
    {
        cout<<"ERROR GET DB LOCK in dbutils getUid\n";
        return 0;
    }
    cout<<"LOCK ACCEPTED\n";    
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
//    std::cout<<"in message \'"<<msg<<"\' we did not fine delimiter \'"<<delimiter<<"\'\n";
    return 0;
}

int DButils::parseParam(string msg,string& param,string& value)
{
    if(parse(msg,delimiter,param,value))
    {
        std::cout<<"param = "<<param<<" value "<<value<<"\n";
        string tmpparam;
        if((parse(param,vardelimiter, tmpparam,param))/*&&(value.length()>3)*/)
        {
    	    std::cout<<"VALUE prepare "<<value<<"\n";
    	    value = value.substr(VALUEPREFIXLENGTH,value.length()-VALUEPOSTFIXLENGTH);
            std::cout<<"VALUE post "<<value<<"\n";
            return 1;
        }
    }
    
    return 0;
}