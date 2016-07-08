#include <FSEventReader.h>
#include <stdio.h>
#include <stdlib.h>
#include <esl.h>
#include <iostream>

using namespace std;


void FSEventReader::start()
{


connector.connect(servhost,servport);


int status = -1;
string data;
while((status=connector.readEvents(data))>=0)
{
    if ((status == 1)&&(!data.empty()))
    {
        boost::thread t(boost::bind(&FSEventReader::processevent,this,data));
         tgroup.add_thread(&t);
         t.detach();
         
    }
    
}


connector.disconnect();

 return;

    
}

string FSEventReader::getMark()
{
    return "Event-Subclass:";
}

FSEventReader::FSEventReader(std::string host,int port,LoggerModule& lm,FSConnector& _connector):
EventReader(host,port,lm),connector(_connector)
{}
