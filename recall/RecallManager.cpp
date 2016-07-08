#include <RecallManager.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

RecallManager::RecallManager()
{
}

int RecallManager::makeAction(ParamMap data,IParser* iparser)
{
    string from;
    string to;
    string announce;
    
    RecallParser* parser = dynamic_cast<RecallParser*>(iparser);
    
    if(parser->parsedata(data,from,to,announce))
    {
	std::cout<<"START CALLING "<<from<<" "<<to<<" "<<announce<<std::endl;
	
        boost::thread(boost::bind(&RecallManager::callWithPause,this,from,to,announce));
    }
}

void RecallManager::callWithPause(string from,string to,string announce)
{
    boost::this_thread::sleep(boost::posix_time::seconds(WaitForSeconds));
    ast.callWithAnnounce(from,to,announce);
}