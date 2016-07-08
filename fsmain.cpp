//
//  main.cpp
//  crmproxy
//
//  Created by Александр Салихов on 24.06.15.
//
//

#include <iostream>
#include <FSEventReader.h>
#include <CRMUrlBuilder.h>
#include <FSParser.h>
#include <RegisterMonitor.h>
#include <RecallManager.h>
#include <MonitorParser.h>
#include <MonitorManager.h>
#include <MonitorParserCRM.h>
#include <LoggerModule.h>
#include <ICMServer.h>
//#include <UtilDButils.h>
#include <CDRManager.h>

using namespace std;

int main()
{
    LoggerModule lm("/var/log/fsproxy_%m%d%Y_%H%M%S_%5N.log");
    UtilDButils DBWorker;
    
    if(DBWorker.getAuthParams("/var/lib/asterisk/agi-bin/system_variables.php"))
    {
        DBWorker.connect();
    }
    else
	std::cout<<"ERROR CREATE DButils object\n";
    
    
    FSConnector connector;
    
    FSEventReader reader("212.193.100.96",8021,lm,connector);
//    FSEventReader reader("213.248.34.111",8021,lm);
    
    CRMUrlBuilder sender("sipuni.com","80",NULL,NULL,NULL);
    FSParser newParser("/api/ats/linefail",lm,DBWorker,connector);
    sender.AddParser(&newParser);
            
    
    reader.AddExecuter(&sender);
    reader.start();
    
    
    return 0;
}