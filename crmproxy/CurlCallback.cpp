#include <CurlCallback.h>

size_t CurlCallback::callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
  int answerSize = size*nmemb;
    callbackParam *cbp = (callbackParam*)userdata;
    DButils* db = (DButils*)(cbp)->db;
    LoggerModule* lm = (LoggerModule*)(cbp)->lm;
    string callid = (cbp)->callid;
    string requestId = (cbp)->requestId;
    delete cbp;

    if(db==NULL)
        return answerSize;

    string answerData(ptr,answerSize);
    string responceId="";

    if(answerSize>0)
    {
        string json(&ptr[1],answerSize-2);

        size_t pos = 0;
        while(((pos = json.find(','))!= std::string::npos)||(!json.empty()))
        {
            string param = "";
            if(pos == std::string::npos)
            {
                param = json;
                json.clear();
            }
            else
            {
                param = json.substr(0, pos);
                json.erase(0, pos + 1);
            }
            if((pos = param.find(':'))!= std::string::npos)
            {
                string paramName = param.substr(1,pos-2);
                string value = (param.erase(0,pos + 1));
                if(paramName.compare("responseId")==0)
                    responceId  = value;
            }
        }

        if(lm!=NULL)
            lm->makeLog(info,"ANSWER DATA "+answerData);
        db->completeEventReportEntry(requestId,responceId,answerData);
    }
    else
        db->completeEventReportEntry("OJ2Ap3yr","ERROR PARSER","ERROR PARSER");
    return answerSize;
}

callbackParam::callbackParam(DButils* _db,string _requestId,string _callid,LoggerModule* _lm):
db(_db),requestId(_requestId),callid(_callid),lm(_lm)
{

}