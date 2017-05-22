#include <EventReader.h>
#include <TestManager.h>
#include <TestParser.h>
#include <LoggerModule.h>
#include <DButils.h>


#include <iostream>

using namespace std;

int main()
{

    LoggerModule lm;
    lm.makeLog(info,"Version 5.01. Log updated");
    DButils DBWorker;
    
    if(DBWorker.getAuthParams("/var/lib/asterisk/agi-bin/system_variables.php"))
     {
              DBWorker.connect();
      }
    else
     {
         std::cout<<"ERROR CREATE DButils object\n";
         return 0;
    }
    TestParser parser("test",lm);
    TestManager manager(&DBWorker);
    manager.AddParser(&parser);
    
    EventReader reader("127.0.0.1",5038,lm);

    reader.AddExecuter(&manager);
    reader.start();
                

}