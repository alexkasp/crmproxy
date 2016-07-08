#include <FSConnector.h>
#include <stdio.h>
#include <iostream>
#include <curl/curl.h>

int FSConnector::connect(string servhost,int servport)
{
    const char* server = servhost.c_str();
    std::cout<<"TRY CONNECT TO "<<server<<"\n";

    esl_connect(&handle,server, servport, NULL, "fjgTR43lodffiroooomnRTeisdkvmmm1riflk59sdfQFasd");
    cout<<"processing\n";

    esl_send_recv(&handle,"api linger");

    esl_send_recv(&handle, "event plain ALL\n\n");
    
    if (handle.last_sr_event && handle.last_sr_event->body)
    {
    	printf("%s\n", handle.last_sr_event->body);
    }
     else 
     {
	    // this is unlikely to happen with api or bgapi (which is hardcoded above) but prefix but may be true for other commands
        printf("%s\n", handle.last_sr_reply);
    }
    return 1;
}
int FSConnector::readEvents(string& data)
{
    int status = 0;
    data.clear();
    status = esl_recv_timed(&handle, 1000);
    if(status == ESL_FAIL)
	return -1;
    
    if (status == ESL_SUCCESS)
    {
        data = handle.last_event->body;
	return 1;
    }

    return 0;
}

void FSConnector::disconnect()
{
    esl_disconnect(&handle);
    
}

int FSConnector::sendRegRequest(string userId,string gateway,string method)
{
    CURL *curl;
    CURLcode res;

    /* get a curl handle */
   curl = curl_easy_init();
    if(curl)
    {
	string curlBaseUrl = "http://extreg04.sipuni.com/IaEQvJmntW/refreshAll.php?userId="+userId+"&lines={\""+gateway+"\":[\""+method+"\"]}";
	std::cout<<"curlBaseUrl "<<curlBaseUrl<<"\n";
	curl_easy_setopt(curl, CURLOPT_URL,curlBaseUrl.c_str());
	res = curl_easy_perform(curl);
    /* Check for errors */
    
	if(res != CURLE_OK)
	{
	    fprintf(stderr, "curl_easy_perform() failed: %s\n",
    	    curl_easy_strerror(res));
    	    curl_easy_cleanup(curl);
	}
	else
	    return 1;
    
    std::cout<<"send url complete\n";
    }

    return 0;
}

int FSConnector::unregLine(string userId,string gateway)
{
    return sendRegRequest(userId,gateway,"DELETE");
}
int FSConnector::regLine(string userId,string gateway)
{
    return sendRegRequest(userId,gateway,"CREATE");
}
                        
