#include <iostream>
#include "EventReader.h"
#include "CRMUrlBuilder.h"
#include "Parser.h"

using namespace std;

int main()
{
	EventReader reader("127.0.0.1",5038);
	
	CRMUrlBuilder sender;
	Parser simpleParser;
	sender.AddParser(&simpleParser);
	reader.SetExecuter(&CRMUrlBuilder());

	reader.start();


	return 0;
}