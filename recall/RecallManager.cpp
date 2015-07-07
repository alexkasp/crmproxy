#include <RecallManager.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

RecallManager::RecallManager()
{
}

int RecallManager::makeAction(ParamMap& data,IParser* iparser)
{
    string from;
    string to;
    
    RecallParser* parser = dynamic_cast<RecallParser*>(iparser);
    
    if(parser->parsedata(data,from,to))
    {
	std::cout<<"START CALLING"<<std::endl;
	
        boost::thread(boost::bind(&RecallManager::callWithPause,this,from,to));
    }
}

void RecallManager::callWithPause(string from,string to)
{
    boost::this_thread::sleep(boost::posix_time::seconds(WaitForSeconds));
    ast.call(from,to);
}