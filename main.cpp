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


using namespace std;

int main()
{
    
    
    EventReader reader("127.0.0.1",5038);
    
    CRMUrlBuilder sender("sipuni.com","80");
    Parser newParser("/ext/crm_api/pbxCrmGatewayHandler?userId=");
    sender.AddParser(&newParser);
    
    
    RegisterParser rparser("not need");
    RegisterMonitor monitor(&rparser,"/var/lib/asterisk/agi-bin/system_variables.php");
    
    RecallManager recallManager;
    RecallParser recall("/IaEQvJmntW/callbackcrm.php?");
    recallManager.AddParser(&recall);
    
    MonitorParser monitorServiceParser("/api/testing/test?callid=");
    MonitorManager monitorServiceManager("sipuni.com","80");
    monitorServiceManager.AddParser(&monitorServiceParser);
    
    
    
    
    //reader.AddExecuter(&sender);
    //reader.AddExecuter(&recallManager);
    //reader.AddExecuter(&monitor);
    reader.AddExecuter(&monitorServiceManager);
    
    reader.start();
    
    
    return 0;
}