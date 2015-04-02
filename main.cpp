#include <iostream>
#include "EventReader.h"
#include "CRMUrlBuilder.h"
#include "Parser.h"

using namespace std;

int main()
{
	EventReader reader("127.0.0.1",5038);
	
	CRMUrlBuilder sender;
	Parser simpleParser("/native/crmtest.php?userId=");
	Parser newParser("/ext/crm_api/pbxCrmGatewayHandler?userId=");

	sender.AddParser(&simpleParser);
	sender.AddParser(&newParser);
	reader.SetExecuter(&sender);

	reader.start();


	return 0;
}