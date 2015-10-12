#include <ICMStorage.h>

ICMStorage::ICMStorage(int _limit):limit(_limit)
{

}

int ICMStorage::getCDRData(string userId,string aNum,CDRData& data)
{
    auto clientMap = aNumToCDRData.find(userId);
    if(clientMap!=aNumToCDRData.end())
    {
        auto numData = clientMap.find(aNum);
        if(numData!=clientMap.end())
        {
            data = numData->second;
            return 1;
        }
    }
    
    return 0;
}
void ICMStorage::putCDRData(string calldate,string userId,string aNum,string operatorNum)
{

}

int putCDRData(map<string,string> rawData)
{
    string userId = rawData["userId"];
    if(!userId.empty())
    {
        auto clientMap = aNumToCDRData.find(userId);
        if(clientMap!=aNumToCDRData.end())
        {
            string clientNum = rawData["clientNum"];
            
            auto numData = clientMap.find(clientNum);
            if(numData!=clientMap.end())
            {
                (clientMap->second).calldate = rawData["calldate"];
                (clientMap->second).operatorNum = rawData["operatorNum"];
            }
            else
            {
                clientMap[clientNum] = {rawData["calldata"],rawData["operatorNum"]};
            }
            
            return 1;
        }
        else
            return -1;
    }
    else
        return -2;
}