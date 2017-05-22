#include <FSParser.h>

string FSRegStatus::UNREGSTATUS = "UNREGED";
string FSRegStatus::REGSTATUS = "REGED";
string FSRegStatus::REGISTERSTATUS = "REGISTER";	


FSRegStatus::FSRegStatus()
{
    counter = 0;
}

string FSParser::unregEvent(string gateway)
{
    string request = request_str;
    gatewayData gwd = gatewaysList[gateway];
    
    if((gwd.uid.empty())||(gwd.login.empty())||(gwd.userId.empty()))
	return "";
    /*
    auto x = registeredList.find(gateway);
    if(x!=registeredList.end())
    {
        registeredList.erase(x);
        deletedList.insert(make_pair(gateway,gwd));
    }
    */
    deletedList.insert(make_pair(gateway,gwd));
    connector.unregLine(gwd.userId,gwd.login);

    
    request+="?status=UNREGED";
    request+= "&uid=";
    request+=gwd.uid;
    request+= "&userId=";
    request+=gwd.userId;
    request+="&login=";
    request+=gwd.login;
    
    return request;		   
}

string FSParser::regEvent(string gateway)
{
    string request = request_str;
    gatewayData gwd = gatewaysList[gateway];
    request+="?status=REGED";
    request+= "&uid=";
    request+=gwd.uid;
    request+= "&userId=";
    request+=gwd.userId;
    request+="&login=";
    request+=gwd.login;
    
    return request;		   
}

string FSParser::gateway_state_parser(ParserData& data)
{
	string request;
		boost::timed_mutex::scoped_lock statusMapList(statusMapLock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
		if(statusMapList)
		{
		    struct timeval now;
		    gettimeofday(&now,NULL);
		    
//		    std::cout<<"INSERT STATUS\n";
		    
		    FSRegStatus fsrs;
		    time_t currentTime = now.tv_sec;
		    fsrs.regTime = currentTime;
		    fsrs.stateString = data["State:"];
		    
		    string gateway = data["Gateway:"];
		    auto it = statusMap.find(gateway);
		    if(it!=statusMap.end())
		    {
			FSRegStatus& currentStatusObj = it->second;
			
			//std::cout<<"PARSE DATA "<<gateway<<" "<<fsrs.stateString<<" "<<currentStatusObj.counter<<"\n";
			
			if((currentStatusObj.stateString.compare(fsrs.stateString)!=0)&&(fsrs.stateString.compare(FSRegStatus::REGISTERSTATUS)!=0))
		    	{
		    	    
		    	    if(fsrs.stateString.compare(FSRegStatus::REGSTATUS)!=0)
		    	    {
		    		if(currentStatusObj.stateString.compare(FSRegStatus::REGSTATUS)==0)
		    		    fsrs.counter = 0;
		    		else if(currentStatusObj.counter<5)
		    	    	    fsrs.counter = ++currentStatusObj.counter;
		    	//	cout<<"INC COUNTER FOR NOT REGED "<<fsrs.counter<<"\n";
		    	    }
		    	//    cout<<"UPDATE REG STATUS\n";
		    	    it->second = fsrs;
		    	    		    	    
		    	//    cout<<"COUNTER OBJECT "<<(it->second).counter<<"\n";
		    	}
		    	else
		    	{
		    	//    cout<<"INC COUNTER\n";
		    	    currentStatusObj.counter++;
		    	}    
		    	
		    	
		    	if(currentStatusObj.stateString==FSRegStatus::REGSTATUS)
		    	{
		    	    if(currentStatusObj.counter==0)
		    	    {
		    	//	std::cout<<"REGSTATUS SEND \n";
		    		request = regEvent(gateway);
		    		auto x = registeredList.find(gateway);
		    		if(!(x!=registeredList.end()))
		    		{
		    		    registeredList.insert(make_pair(gateway,FSRegStatus::REGSTATUS));
		    		}
		    		if(currentStatusObj.counter<5)
		    		    currentStatusObj.counter++;
		    	    }
		    	}
		    	else
		    	{
		    	//    std::cout<<"NOT REGISTERED STATUS\n";
		    	    int maxtrycount = 3;
		    	    auto gwd = gatewaysList.find(gateway);
		    	    if(gwd!=gatewaysList.end())
		    		maxtrycount = (gwd->second).maxtrycount;
		    		
		    	    if(currentStatusObj.counter==maxtrycount)
		    	    {
		    		std::cout<<"UNREG!!!!!! SEND \n";
		    	    
		    		request = unregEvent(gateway);
		    	    
		    	    
		    	     
		    	    }
		    	}
		    }
		    else
		    {
			fsrs.counter++;
			statusMap.insert(make_pair(gateway,fsrs));
		    }
		    
		    
		    /*
		    for(auto it=statusMap.begin();it!=statusMap.end();++it)
		    {
			std::cout<<"statusMap "<<(it->first)<<"  "<<(it->second).stateString<<"\n";
		    }*/
		}
	return request;
}

string FSParser::parsedata(ParserData& data)
{
    string request = "";
    try{
	    for(auto it=data.begin();it!=data.end();++it)
		lm.makeLog(boost::log::trivial::severity_level::info,"DATA "+(it->first)+"  "+(it->second));
		
	    
    
	    string str = "";
	//    std::cout<<"data[\"Event-Subclass:\"] = "<<data["Event-Subclass:"]<<"\n";
	    
	    
	    if(data["Event-Subclass:"]=="sofia%3A%3Agateway_state")
	    {
		request = gateway_state_parser(data);
		
	    }
	}
     catch(exception &ec)
	{
	    string errmsg = "Error in FSParser::parsedata ";
	    lm.makeLog(boost::log::trivial::severity_level::error,errmsg+ec.what());
	    std::cout<<errmsg<<"\n "<<ec.what()<<"\n";
	 }
	         
    return request;
}

FSParser::FSParser(const string str,LoggerModule& _lm,DButils& _DBWorker,FSConnector& _connector,std::string _extregid):IParser(str,_lm),DBWorker(_DBWorker),connector(_connector),extregid(_extregid)
{
    (dynamic_cast<UtilDButils&>(DBWorker)).loadGateways(gatewaysList,extregid);
    
/*    for(auto x = gatewaysList.begin();x!=gatewaysList.end();)
    {
	gatewayData gwd = x->second;
	std::cout<<" recreate Line "<<gwd.login<<"\n";
	connector.unregLine(gwd.userId,gwd.login);
	
	connector.regLine(gwd.userId,gwd.login);
	x++;
    
    }
    */
    tgroup.create_thread(boost::bind(&FSParser::CheckStateCicle,this));
    tgroup.create_thread(boost::bind(&FSParser::undelLine,this));
    

}


FSParser::~FSParser()
{}

void FSParser::undelLine()
{
    try{
	while(1)
	{
	    boost::this_thread::sleep( boost::posix_time::milliseconds(10000));
	    //std::cout<<"UNDEL LINE CICLE\n";
	    for(auto x = deletedList.begin();x!=deletedList.end();)
	    {
		gatewayData& gwd = x->second;
		string gateway = x->first;
		
		//std::cout<<"undelLine "<<gwd.login<<" ("<<gwd.buntime<<")\n";
		
		if(--gwd.buntime==0)
		{
		    connector.regLine(gwd.userId,gwd.login);
		    registeredList.insert(make_pair(gateway,FSRegStatus::REGSTATUS));
		    deletedList.erase(x++);
		}
		else
		    x++;
    	    }
	}
    
    }
    catch(exception &ec)
        {
        std::string errmsg = "Error in undelLine ";
        std::cout<<errmsg<<" "<<ec.what()<<"\n";
            }
}

void FSParser::CheckStateCicle()
{
    const int refreshCounter=3;
    int indexCounter=0;
    try
    {
	while(1)
	{
	    boost::this_thread::sleep( boost::posix_time::milliseconds(10000));
	    boost::timed_mutex::scoped_lock statusMapList(statusMapLock,boost::get_system_time() + boost::posix_time::milliseconds(10000));
	    if(statusMapList)
	    {
		indexCounter++;
		if(indexCounter>=refreshCounter)
		{
		    indexCounter=0;
		    
		    //refresh object list
		    (dynamic_cast<UtilDButils&>(DBWorker)).loadGateways(gatewaysList,extregid);
		}
		for(auto it=statusMap.begin();it!=statusMap.end();++it)
		{
		
		    gatewayData gateway = gatewaysList[it->first];
		    if(!gateway.gatewayname.empty()&&(((gateway.status.compare("REGED")!=0)&&((it->second).stateString.compare("REGED")==0))||((gateway.status.compare("REGED")==0)&&((it->second).stateString.compare("REGED")!=0))))
		    {
			(dynamic_cast<UtilDButils&>(DBWorker)).updateStaticProvs(gateway.userId,gateway.gatewayname,(it->second).stateString);
		    }
		}
	    }
	}
    }
    catch(exception &ec)
        {
        std::string errmsg = "Error in checkStateCicle ";
        std::cout<<errmsg<<" "<<ec.what()<<"\n";
            }
}