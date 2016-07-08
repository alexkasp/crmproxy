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

int UtilDButils::loadGateways(map<string,gatewayData>& gateways)
{
    boost::mutex::scoped_lock Lock(dblock);
    mysqlpp::Query query = conn->query();
    query << "select login,type,userId,uid,status from static_provs";
//    std::cout<<query.str();
    if (mysqlpp::StoreQueryResult res = query.store())
    {
      for(auto it=res.begin();it!=res.end();++it)
      {	
    	
    	    mysqlpp::Row row = *it;
    	//    std::cout<<"CDR DATA "<<row[0].data()<<"//"<<row[1].data()<<"//"<<row[2].data()<<"\n";
    	    string gatewayname=row[0].data();
    	    gatewayname+=row[2].data();
    	    
    	    gatewayData gwd;
    	    gwd.gatewayname = row[0].data();
    	    gwd.userId = row[2].data();
    	    gwd.uid = row[3].data();
    	    gwd.login = row[0].data();
    	    gwd.type = row[1].data();
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
    boost::mutex::scoped_lock Lock(dblock);
    
    string querystr="";
    if(status=="REGED")
	querystr="update static_provs set status = 1 where login ='"+login+"' and userid="+userId;
    else
	querystr="update static_provs set status = 0 where login ='"+login+"' and userid="+userId;

    

   mysqlpp::Query query = conn->query(querystr);

//    std::cout<<query.str()<<"\n";
    
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
