#include <astmanager.h>
#include <DButils.h>
#include <iostream>

using namespace std;

int main()
{
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
     
    AsteriskManager ast;
    string a;
    string from,to;
    
    while(1)
    {
	std::cout<<"Enter test id or 0 for exit\n";
	cin>>a;
	if(a=="0")
	    return 0;
	std::cout<<"You entered "<<a<<"\n";
	if(DBWorker.getTestById(a,from,to))
	{
	    ast.onewaycall(from,to,a);
	}
	else
	{
	    std::cout<<"Error get Test by ID\n";
	    return 0;
	}
	
    }
    return 0;
}