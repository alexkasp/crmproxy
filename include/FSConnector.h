#ifndef FSCONNECTOR_HEADER
#define FSCONNECTOR_HEADER 1

#include <esl.h>
#include <string>


using namespace std;

class FSConnector
{
    esl_handle_t handle = {{0}};
    	int sendRegRequest(string userId,string gateway,string method);
    	std::string servurl;
	
    public:
	FSConnector(std::string servurl);
	int connect(string server,int serverport);
	int readEvents(string& data);
	int unregLine(string userId,string gateway);
	void disconnect();
	int regLine(string userId,string gateway);

};

#endif