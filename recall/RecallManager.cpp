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
    string channel;
    string dialstr;
    string dialtime;
    string dialargs;
    string callernum;
    
    RecallParser* parser = dynamic_cast<RecallParser*>(iparser);
    
    if(parser->parsedata(data,from,to,announce))
    {
	std::cout<<"START CALLING "<<from<<" "<<to<<" "<<announce<<std::endl;
	
        boost::thread(boost::bind(&RecallManager::callWithPause,this,from,to,announce));
    }
    else if(parser->parsedatacheckanswer(data,from,to,channel,callernum,dialstr,dialtime,dialargs))
    {
	std::cout<<"START CHECKASNWER "<<from<<" "<<to<<" "<<announce<<std::endl;
	
        boost::thread(boost::bind(&RecallManager::callCheckAnswer,this,from,to,channel,callernum,dialstr,dialtime,dialargs));
    }
    
}

void RecallManager::callWithPause(string from,string to,string announce)
{
    boost::this_thread::sleep(boost::posix_time::seconds(WaitForSeconds));
    ast.callWithAnnounce(from,to,announce);
}

void RecallManager::callCheckAnswer(string from,string to,string channel,string callernum,string dialstr,string dialtime,string dialargs)
{
    ast.callCheckAnswer(from,to,channel,callernum,dialstr,dialtime,dialargs);
}