#include <ICMStorage.h>
#include <iostream>

ICMStorage::ICMStorage(int _limit):limit(_limit)
{

}

int ICMStorage::getCDRData(string userId,string aNum,CDRData& data)
{
    std::cout<<"getCDRDATA "<<userId<<" "<<aNum<<"\n";
    
    auto clientMap = aNumToCDRData.find(userId);
    if(clientMap!=aNumToCDRData.end())
    {
        auto numData = (clientMap->second).find(aNum);
        if(numData!=(clientMap->second).end())
        {
            data = numData->second;
            std::cout<<"FIND operator "<<data.operatorNum<<" :  "<<data.calldate<<" : "<<data.callid<<" from  "<<data.clientNum<<"\n";
            return 1;
        }
        std::cout<<"NO SUCH NUMBER\n";
        return 0;
    }
    std::cout<<"NOT USER CALL at all\n";
    return 0;
}
int ICMStorage::putCDRData(string calldate,string userId,string aNum,string operatorNum,string callid)
{

    if(!userId.empty())
    {
        auto clientMap = aNumToCDRData.find(userId);
        if((clientMap!=aNumToCDRData.end())&&(((clientMap->second).size()<limit)))
        {
    	    std::cout<<"NOT FIRST CALL\n";
            
            
        }
        else
        {
            if((clientMap!=aNumToCDRData.end())&&((clientMap->second).size()>limit))
            {
        	std::cout<<"ERASE MAP because limit\n";
                aNumToCDRData.erase(clientMap);
            }
    	    map<string,CDRData> tmpMap;
    	    
    	    std::cout<<"try insert callMap\n";
    	    auto insertResult = aNumToCDRData.insert(make_pair(userId,tmpMap));
    	    
    	    clientMap = insertResult.first;
    	    if(clientMap!=aNumToCDRData.end())
    		std::cout<<"SUCCES ADD ELEMENT\n";
    	    else
    		std::cout<<"ERROR ADD ELEMENT\n";
        }
        
        if(clientMap!=aNumToCDRData.end())
        {
            string clientNum = aNum;
            
            auto numData = (clientMap->second).find(clientNum);
            if(numData!=(clientMap->second).end())
            {
                (numData->second).calldate = calldate;
                (numData->second).operatorNum = operatorNum;
                (numData->second).callid = callid;
                (numData->second).clientNum = aNum;
                std::cout<<"UPDATE CDRDATA "<<(numData->second).calldate<<" "<<(numData->second).operatorNum<<" "<<(numData->second).callid<<" "<<(numData->second).clientNum<<"\n";
            }
            else
            {
        	CDRData cdr(calldate,operatorNum,callid);
        	std::cout<<"ADD CDRDATA "<<calldate<<" "<<operatorNum<<" "<<callid<<" for "<<clientNum<<"\n";
                (clientMap->second).insert(make_pair(clientNum,cdr));
            }
            
            return 1;
        }
        else
            return -1;
    }
    else
        return -2;
}

int ICMStorage::putCDRData(map<string,string> rawData)
{
    for(auto x=rawData.begin();x!=rawData.end();++x)
	std::cout<<(x->first)<<" : "<<(x->second)<<"\n";
    return putCDRData(rawData["call_start_timestamp"],rawData["userId"],rawData["src_num"],rawData["dst_num"],rawData["UniqueID"]);
}