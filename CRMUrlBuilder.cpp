#include "CRMUrlBuilder.h"

#include <curl/curl.h>

int CRMUrlBuilder::Execute(ParamMap& data)
{
	string request = parser.parsedata(data);
	if(request.size()>0)
		return SendRequest(request);
	else
		return 0;
}

int CRMUrlBuilder::SendRequest(std::string url)
{
	
	CURL *curl;
	CURLcode res;
 
	curl = curl_easy_init();
	if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
   
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
	return 1;
  }
	return 0;
}