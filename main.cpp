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

using namespace std;

int main()
{
    
    LoggerModule lm;
    
    EventReader reader("127.0.0.1",5038,lm);
    
    CRMUrlBuilder sender("sipuni.com","80");
    Parser newParser("/ext/crm_api/pbxCrmGatewayHandler?userId=",lm);
    sender.AddParser(&newParser);
    
    RegisterParser rparser("not need",lm);
    RegisterMonitor rmonitor("/var/lib/asterisk/agi-bin/system_variables.php");
    rmonitor.AddParser(&rparser);
    
    RecallManager recallManager;
    RecallParser recall("/IaEQvJmntW/callbackcrm.php?",lm);
    recallManager.AddParser(&recall);
    
    MonitorParser monitorServiceParser("/api/testing/record?callId=",lm);
    MonitorParserCRM monitorServiceParserCRM("/api/testing/crm?callId=","/var/lib/asterisk/agi-bin/system_variables.php",lm);
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