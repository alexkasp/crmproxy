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

using namespace std;

int main()
{
    
    LoggerModule lm;
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
    EventReader reader("127.0.0.1",5038,lm);
    
    CRMUrlBuilder sender("sipuni.com","80",&icm);
    Parser newParser("/ext/crm_api/pbxCrmGatewayHandler?userId=",lm);
    sender.AddParser(&newParser);
    
    RegisterParser rparser("not need",lm);
    RegisterMonitor rmonitor(DBWorker);
    rmonitor.AddParser(&rparser);
    
    RecallManager recallManager;
    RecallParser recall("/IaEQvJmntW/callbackcrm.php?",lm);
    recallManager.AddParser(&recall);
    
    MonitorParser monitorServiceParser("/api/testing/record?callId=",lm);
    MonitorParserCRM monitorServiceParserCRM("/api/testing/crm?callId=",DBWorker,lm);
    MonitorManager monitorServiceManager("sipuni.com","80");
    monitorServiceManager.AddParser(&monitorServiceParser);
    monitorServiceManager.AddParser(&monitorServiceParserCRM);
    
    
    
    
    reader.AddExecuter(&sender);
    reader.AddExecuter(&recallManager);
    reader.AddExecuter(&rmonitor);
    reader.AddExecuter(&monitorServiceManager);
    
    reader.start();
    
    
    return 0;
}