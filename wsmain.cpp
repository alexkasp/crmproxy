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
#include <WSSMonitor.h>
//#include <UtilDButils.h>
#include <CDRManager.h>

using namespace std;

int main()
{
    string webservaddr = "wss.sipuni.com";
    LoggerModule lm("/var/log/wssmonitor_%m%d%Y_%H%M%S_%5N.log");
    DButils DBWorker;
    
    
    
    if(DBWorker.getAuthParams("/var/lib/asterisk/agi-bin/system_variables.php"))
    {
        DBWorker.connect();
    }
    else
	std::cout<<"ERROR CREATE DButils object\n";
    
    
    CDRManager cdr(lm,webservaddr,"80","http://wss.sipuni.com:9204","/ext/keepalive",DBWorker);
    CRMUrlBuilder sender(webservaddr,"9202",&DBWorker);

    WSSMonitor wsmon(DBWorker,sender,cdr);
    wsmon.start();
    wsmon.unstop();
    return 0;
}