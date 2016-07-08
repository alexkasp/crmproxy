#include "EventReader.h"
#include "FSConnector.h"

class FSEventReader: public EventReader
{
	boost::thread_group tgroup;
	FSConnector& connector;
    protected:
	virtual string getMark();
    
    public:
	FSEventReader(std::string host,int port,LoggerModule& lm,FSConnector& connector);
	void start();
};