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


    //string webservaddr = "212.193.100.104";
    string webservaddr = "wss.sipuni.com";
//    string webservaddr = "178.124.128.205";
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
    
    
    ICMServer icm(lm,DBWorker);
    if(!icm.init(7722))
	std::cout<<"ERROR START ICM SERVER!!!!!\n";
    else
    {
	icm.startProcessing();
	std::cout<<"ICM SERVER OK\n";
    }
    
    CDRManager cdr(lm,webservaddr,"80","http://"+webservaddr+":9104","/ext/keepalive",DBWorker);
//    CDRManager cdr(lm,"127.0.0.1","80","/IaEQvJmntW/autocall.php","/ext/keepalive");
    
    
    EventReader reader("127.0.0.1",5038,lm);
    
    CRMUrlBuilder sender(webservaddr,"9102",&DBWorker,&icm,&cdr/*NULL*/);
    //Parser newParser("/ext/crm_api/pbxCrmGatewayHandler?userId=",lm);
    Parser newParser("/?userId=",lm);
    sender.AddParser(&newParser);
    
    RegisterParser rparser("not need",lm);
    RegisterMonitor rmonitor(DBWorker);
    rmonitor.AddParser(&rparser);
    
    RecallManager recallManager;
    RecallParser recall("/IaEQvJmntW/callbackcrm.php?",lm);
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