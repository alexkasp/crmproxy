//
//  main.cpp
//  crmproxy
//
//  Created by Александр Салихов on 24.06.15.
//
//

#include <iostream>
#include <EventReader.h>
#include <CRMUrlBuilder.h>
#include <Parser.h>
#include <RegisterMonitor.h>
#include <RecallManager.h>
#include <MonitorParser.h>
#include <MonitorManager.h>
#include <MonitorParserCRM.h>
#include <LoggerModule.h>
#include <ICMServer.h>
#include <DButils.h>
#include <CDRManager.h>
#include <regex>
#include <string>
using namespace std;



using namespace std;

int main()
{


//    string webservaddr = "wss.sipuni.com";
    string webservaddr = "crmstat.sipuni.com";
    cout<<"CDR by NET version 4.4(DB lock withou exit)\n";
    
    LoggerModule lm;
    lm.makeLog(info,"Version 5.01. Log updated");
    DButils DBWorker;
    
    if(DBWorker.getAuthParams("/var/lib/asterisk/agi-bin/system_variables.php"))
    {
        DBWorker.connect();
    }
    else
	std::cout<<"ERROR CREATE DButils object\n";
    
    
    std::cout<<"Server ID = "<<std::stoi(DBWorker.getPBXServerId())<<"\n";
    int serverInInt = std::stoi(DBWorker.getPBXServerId());
/*    if( serverInInt >= 122)
    {
	std::cout<<"We set test WSS\n";
	webservaddr = "wss3.sipuni.com";
    }
*/    
    if(DBWorker.getPBXServerId() == "29")    
    {
	std::cout<<"We set test WSS\n";
	webservaddr = "wss2test.sipuni.com";
    
    }
    
    int asterVersion = 13;
//    if(DBWorker.getPBXServerId() == "81")
//	asterVersion = 11;
    
    
    std::cout<<"We set ASTER version = "<<asterVersion<<"\n";
    
    ICMServer icm(lm,DBWorker);
    if(!icm.init(7722))
	std::cout<<"ERROR START ICM SERVER!!!!!\n";
    else
    {
	icm.startProcessing();
	std::cout<<"ICM SERVER OK\n";
    }
    
    std::cout<<"webservaddr="<<webservaddr<<"\n";
    
//    CDRManager cdr(lm,webservaddr,"80","http://"+webservaddr+":9104","/ext/keepalive",DBWorker);
CDRManager cdr(lm,webservaddr,"80","http://"+webservaddr+":80","/ext/keepalive",DBWorker);
    
    
    EventReader reader("127.0.0.1",5038,lm);
    
//    CRMUrlBuilder sender(webservaddr,"9102",&DBWorker,&icm,&cdr/*NULL*/);
CRMUrlBuilder sender(webservaddr,"80",&DBWorker,&icm,&cdr,&lm);
    Parser newParser("/?userId=",lm);
    newParser.addDBWorker(&DBWorker);
    newParser.setAsterVer(asterVersion);
    
    sender.AddParser(&newParser);
    
    RegisterParser rparser("not need",lm);
    RegisterMonitor rmonitor(DBWorker);
    rmonitor.AddParser(&rparser);
    
    RecallManager recallManager;
    RecallParser recall("/IaEQvJmntW/callbackcrm.php?",lm);
    recall.setAsterVer(asterVersion);
    recallManager.AddParser(&recall);
    
//    MonitorParser monitorServiceParser("/api/testing/record?callId=",lm);
//    MonitorParserCRM monitorServiceParserCRM("/api/testing/crm?callId=",DBWorker,lm);
//    MonitorManager monitorServiceManager(webservaddr,"80",&DBWorker);
//    monitorServiceManager.AddParser(&monitorServiceParser);
//    monitorServiceManager.AddParser(&monitorServiceParserCRM);
    
    
    
    
    reader.AddExecuter(&sender);
    reader.AddExecuter(&recallManager);
    //reader.AddExecuter(&rmonitor);
    //reader.AddExecuter(&monitorServiceManager);
    
    reader.start();
    
    lm.makeLog(info,"We EXIT unexpectedly");
    return 0;
}