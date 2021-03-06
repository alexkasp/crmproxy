#include <UtilDButils.h>

string UtilDButils::getHostParamName()
{
    return "ExtregHost";
}
    
string UtilDButils::getPassParamName()
{
    return "ExtregPass";
}
        
string UtilDButils::getUserParamName()
{
    return "ExtregUser";
}
            
string UtilDButils::getDBParamName()
{
    return "ExtregDB";
}

int UtilDButils::loadGateways(map<string,gatewayData>& gateways,std::string extregid)
{
    //boost::mutex::scoped_lock Lock(dblock);

boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
        {
                cout<<"ERROR GET DB LOCK in dbutils\n";
                        exit(-1);
                            }
                                cout<<"LOCK ACCEPTED\n";
                                
    mysqlpp::Query query = conn->query();
    query << "select login,type,userId,uid,status,buntime,id from static_provs where extservid="<<extregid<<" and type <> 'SipRedirect' order by login";
    std::cout<<query.str();
    if (mysqlpp::StoreQueryResult res = query.store())
    {
      for(auto it=res.begin();it!=res.end();++it)
      {	
    	
    	    mysqlpp::Row row = *it;
    	    std::cout<<"CDR DATA "<<row[0].data()<<"//"<<row[1].data()<<"//"<<row[2].data()<<"\n";
    	    string gatewayname=row[6].data();
    	    gatewayname+=row[2].data();
    	    
    	    gatewayData gwd;
    	    gwd.gatewayname = row[0].data();
    	    gwd.userId = row[2].data();
    	    gwd.uid = row[3].data();
    	    gwd.login = row[0].data();
    	    gwd.type = row[1].data();
    	    
    	    gwd.maxtrycount=3;
    	    gwd.buntime=row[5];
    	    gwd.Id = row[6].data();
    	    
    	    if(strcmp(row[4].data(),"1")==0)
        	gwd.status = "REGED";
	    else
		gwd.status = "UNREG";	    
    	    gateways[gatewayname]=gwd;
	}
    }
    return 0;
}
                
int UtilDButils::updateStaticProvs(string userId,string login,string status)
{
//    boost::mutex::scoped_lock Lock(dblock);
boost::timed_mutex::scoped_lock Lock(dblock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
    if(!Lock)
        {
                cout<<"ERROR GET DB LOCK in dbutils\n";
                        exit(-1);
                            }
                                cout<<"LOCK ACCEPTED\n";
                                

    string querystr="";
    if(status=="REGED")
	querystr="update static_provs set status = 1 where login ='"+login+"' and userid="+userId;
    else
	querystr="update static_provs set status = 0 where login ='"+login+"' and userid="+userId;

    

   mysqlpp::Query query = conn->query(querystr);

    std::cout<<query.str()<<"\n";
    
    try
    {	
	query.execute();
/*        if (mysqlpp::StoreQueryResult res = query.store())
         {
             std::cout<<"success query\n";
             
                  }

                      else
                          {
                                  std::cout << "Failed to updateStaticProvs: " << query.error() << endl;
                                  
                                              }
*/	}
	catch(exception &ec)
	{
	    string errmsg = "Error in read_handle ";
	    std::cout<<errmsg<<" "<<ec.what()<<"\n";
	    return 0;
        }
    return 1;
}
