#ifndef CURL_CALLBACK_MODULE
#define CURL_CALLBACK_MODULE

#include <DButils.h>
#include <LoggerModule.h>
#include <string>

class callbackParam
{
    callbackParam(){};
    callbackParam(callbackParam&){};
    
    public:
    DButils* db;
    string requestId;
    LoggerModule* lm;
    string callid;
    
    callbackParam(DButils* _db,string _requestId,string _callid,LoggerModule* _lm);
};

class CurlCallback
{

    public:
	size_t callback(char *ptr, size_t size, size_t nmemb, void *userdata);
};

#endif