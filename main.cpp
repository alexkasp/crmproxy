#include <iostream>
#include "EventReader.h"
#include "CRMUrlBuilder.h"
#include "Parser.h"
#include "RegisterMonitor.h"

using namespace std;

int main()
{
	
	
	EventReader reader("127.0.0.1",5038);
	
	CRMUrlBuilder sender;
	Parser newParser("/ext/crm_api/pbxCrmGatewayHandler?userId=");
		
	RegisterParser rparser("not need");
	RegisterMonitor monitor(&rparser,"/var/lib/asterisk/agi-bin/system_variables.php");
	
	sender.AddParser(&newParser);
	
	
	reader.AddExecuter(&sender);
    	reader.AddExecuter(&monitor);
	
	reader.start();


	return 0;
}