#include <curl/curl.h>
#include <CDRManager.h>
#include <iostream>
#include <sstream>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>



std::string cdrchars(
        "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "1234567890");
                
    
boost::random::random_device cdrrng;
boost::random::uniform_int_distribution<> cdrindex_dist(0, cdrchars.size() - 1);

struct callbackParam
{
    DButils* db;
    string requestId;
};
    
size_t cdr_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    callbackParam *cbp = (callbackParam*)userdata;
    
    DButils* db = (DButils*)(cbp)->db;
    string requestId = (cbp)->requestId;
    
    delete cbp;
    
    //{"status":true,"responseId":"146487439569361101"}
    
    int answerSize = size*nmemb;
    
    string answerData(ptr,answerSize);
    
    string responceId="";
    
    if(answerSize>0)
    {
	string json(&ptr[1],answerSize-2);
    
	size_t pos = 0;
	while(((pos = json.find(','))!= std::string::npos)||(!json.empty()))
	{
//	    std::cout<<"PARSE MSG "<<json<<"\n";
	
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
    		
    		//std::cout<<"PARAMNAME = "<<paramName<<"  value = "<<value<<"\n";
    		
    		if(paramName.compare("responseId")==0)
    		    responceId  = value;
    	    }
	}
    
    
	db->completeEventReportEntry(requestId,responceId,answerData);
    }
    else
	db->completeEventReportEntry("OJ2Ap3yr","ERROR PARSER","ERROR PARSER");
//    std::cout<<"WRITE_CALLBACK\n";
    return answerSize;
}

void CDRManager::keepAlive()
{
    while(1)
    {   boost::this_thread::sleep( boost::posix_time::milliseconds(10000));
	try{
    	    boost::asio::streambuf request;
    	    std::ostream request_stream(&request);
    	    request_stream << "GET " << keepAliveUrl << " HTTP/1.0\r\n";
    	    request_stream << "Host: " << server << "\r\n";
    	    request_stream << "User-Agent: C/1.0\r\n";
    	    request_stream << "Content-Type: application/x-www-form-urlencoded\r\n";
    	    request_stream << "Accept: */*\r\n";
        
        
    	    boost::system::error_code ec;
    	    //boost::asio::write(sock,request);
        
    	    boost::shared_ptr<boost::asio::streambuf> buf = boost::shared_ptr<boost::asio::streambuf>(new boost::asio::streambuf());
	    //boost::asio::async_read_until(sock,*buf,"\r\n\r\n",boost::bind(&CDRManager::read_handler,this,buf,_1,_2));
	}
	catch (exception& e)
	{
    	    cout<<"CATCH EXCEPTION!!!" << e.what() << '\n';
	}
    }
    return;
}

CDRManager::CDRManager(LoggerModule& _lm,string webserver,string port,string _baseUrl,string _keepAliveUrl,DButils& _db):lm(_lm),baseUrl(_baseUrl),keepAliveUrl(_keepAliveUrl),db(_db)
{
    server = webserver;
    baseUrl = _baseUrl;
    
    boost::asio::ip::tcp::resolver resolver(io);
    boost::asio::ip::tcp::resolver::query query(server,port);
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve( query);
    
    ep = *iter;
    
    std::cout << ep.address().to_string() << std::endl;
    
    
//    boost::thread t(boost::bind(&CDRManager::keepAlive,this));
//    t.detach();
}

CDRManager::~CDRManager()
{
//    sock.close();
}

void CDRManager::processCDR(map<string,string> data)
{
    std::cout<<"process CDR\n";
    
    for(auto x=data.begin();x!=data.end();++x)
	std::cout<<(x->first)<<" : "<<(x->second)<<"\n";
    
    if(data["event"]=="2")
    {
	std::cout<<"CDRManager::processCDR -> DBWorker.putCDR\n";
	putCDR(data);
    }
    else if(data["event"]=="3")
    {
	addInvolvedNums(data);
    }
    else
	std::cout<<"not 2 or 3 event\n";
}

void CDRManager::addInvolvedNums(map<string,string>& data)
{
    std::cout<<"add involved nums\n";
    
    string callid = data["call_id"];
    string answeredNum = data["dst_num"];
    
    auto x = involvedNums.find(callid);
    if(x!=involvedNums.end())
    {
	vector<string>& nums = x->second;
	nums.push_back(answeredNum);
    }
    else
    {
	vector<string> nums;
	nums.push_back(answeredNum);
	involvedNums.insert(make_pair(callid,nums));
    }
    
    
}

void CDRManager::putCDR(map<string,string> data)
{
    std::cout<<"make query\n";
    string callid = data["call_id"];
    string treeId = data["TreeId"];
    if(treeId.empty())
	treeId="0";
    
    string src = data["src_num"];
    //if(data["calltype"]=="out")
//	src = data["dst_num"];
    
    string dst = data["destination"];
    string answernum = data["dst_num"];
    
    string uidcode = data["uidcode"];
    
    if((src.length()==10)&&(!uidcode.empty())&&(src.substr(0,uidcode.length()).compare(uidcode)!=0))
	{
	    
	    src = "8"+src;
	}
	else if((src.length()>11)&&(src.substr(0,3)=="100"))
	{
	    src = src.substr(3, string::npos);
	    src = "8"+dst;
	}
    if((dst.length()>11)&&(dst.substr(0,3)=="100"))
	{
	    dst = dst.substr(3, string::npos);
	    dst = "8"+dst;
	}
	
    string recordName = data["call_record_link"];
    int numRecords = !(recordName.empty());

    std::cout<<"Process involvedNums\n";
    for(auto x=involvedNums.begin();x!=involvedNums.end();++x)
    {
    	std::cout<<(x->first)<<"\n";
    	
    	for(auto nums=(x->second).begin();nums!=(x->second).end();++nums)
    	    std::cout<<(*nums)<<"\n";
    }
    string answeredNums = "";
    auto x = involvedNums.find(callid);
    if(x!=involvedNums.end())
    {
	std::cout<<"start calculate answerednums\n";
	vector<string>& nums = x->second;
	
	for(auto num=nums.begin();num!=nums.end();++num)
	{
	    answeredNums+=(*num);
	    answeredNums+=",";
	    std::cout<<answeredNums<<"\n";
	    answernum = (*num);
	}
	
	boost::trim_right_if(answeredNums,boost::is_any_of(","));
	involvedNums.erase(x);
    }
	
	std::cout<<"Stop process involvedNums\n";
    
    string status = data["status"];
    
    if(answeredNums!="")
    {
	status = "ANSWERED";
    }
    else
    {
	
	if(status=="ANSWER")
	    status = "ANSWERED";
	else if(status == "CHANUNAVAIL")
	    status = "NO ANSWER";
	else if(status == "NOANSWER")
	    status = "NO ANSWER";
    }
    
    map<string,string> CDRData;
    
    map<string,string> additionalData;
    db.getCDR(callid,additionalData);
    
    CDRData["calldate"] = data["call_start_timestamp"];
    CDRData["src"] = src;
    CDRData["dst"] = dst;
    CDRData["to2"] = answernum;
    CDRData["duration"] = data["duration"];
    CDRData["billsec"] = data["billableseconds"];
    CDRData["disposition"] = status;
    CDRData["uniqueid"] = callid;
    CDRData["numrecords"] = boost::lexical_cast<std::string>(numRecords);
    CDRData["recordName"] = recordName;
    CDRData["answertime"] = data["call_answer_timestamp"];
    CDRData["treeId"] = treeId;
    CDRData["userId"] = data["userId"];
    CDRData["context"] = data["DestinationContext"];
    CDRData["serverId"] = data["serverId"];
    CDRData["isBlock"] = additionalData["isblock"];
    CDRData["callapiorder"] = data["callbackId"];
    CDRData["numbersInvolved"] = answeredNums;
    CDRData["label"] = additionalData["label"];
    CDRData["rating"] = additionalData["raiting"];  
    CDRData["newstatus"] = additionalData["newstatus"];    
    
    auto it = data.find("transfercallid");
    if(it!=data.end())
    {
	std::cout<<"FIND TRANSFERED CALL "<<(it->second)<<"\n";
	CDRData["uniqueid"]=it->second;
    }
    
    it = data.find("transferrecord");
    if(it!=data.end())
    {
	std::cout<<"FIND TRANSFERED RECORD "<<(it->second)<<"\n";
	CDRData["recordName"]=it->second;
    }
    string requestId = "";
    for(int i = 0; i < 8; ++i) {
    requestId += cdrchars[cdrindex_dist(cdrrng)];
    }
    
    CDRData["requestId"]=requestId;
    
//    sendCurlRequest(map2json(CDRData),requestId);
    
    string sendData = map2json(CDRData);
    
    
    db.addSendEventReportEntry(CDRData["uniqueid"],CDRData["requestId"],CDRData["serverId"],CDRData["userId"],"1",sendData);
    sendJsonRequest(sendData,CDRData["requestId"]);
}

void CDRManager::sendCurlRequest(string url,string requestId)
{
     CURL *curl;
       CURLcode res;
       
      /* get a curl handle */ 
    curl = curl_easy_init();
     if(curl) {
     /* First set the URL that is about to receive our POST. This URL can
     just as well be a https:// URL if that is what should receive the
                data. */ 
    string curlBaseUrl = "http://sipuni.com"+baseUrl+"?requestId="+requestId;
    std::cout<<"curlBaseUrl "<<curlBaseUrl<<"\n";
    curl_easy_setopt(curl, CURLOPT_URL,curlBaseUrl.c_str());
    /* Now specify the POST data */ 
    string curlUrl = "params="+url;
    
    std::cout<<"curlUrl "<<curlUrl<<std::endl;
    
    
    
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curlUrl.c_str());
//    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
                             
     /* Perform the request, res will get the return code */ 
      res = curl_easy_perform(curl);
     /* Check for errors */ 
      if(res != CURLE_OK)
         fprintf(stderr, "curl_easy_perform() failed: %s\n",
      curl_easy_strerror(res));
      curl_easy_cleanup(curl);
    }
    std::cout<<"send url complete\n";
}

void CDRManager::sendJsonRequest(string url,string requestId)
{
     CURL *curl;
       CURLcode res;
       
      /* get a curl handle */ 
    curl = curl_easy_init();
     if(curl) {
     /* First set the URL that is about to receive our POST. This URL can
     just as well be a https:// URL if that is what should receive the
                data. */ 
    string curlBaseUrl = baseUrl;//"http://wss.sipuni.com:9104";
    std::cout<<"curlBaseUrl "<<curlBaseUrl<<"\n";
    curl_easy_setopt(curl, CURLOPT_URL,curlBaseUrl.c_str());
    /* Now specify the POST data */ 
    string curlUrl = url;
    
    std::cout<<"curlUrl "<<curlUrl<<std::endl;
    
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curlUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cdr_write_callback);
    
    callbackParam* sendParam =new callbackParam();
    
    sendParam->db=&db;
    sendParam->requestId = requestId;

    
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, sendParam);
                                 
     /* Perform the request, res will get the return code */ 
      res = curl_easy_perform(curl);
     /* Check for errors */ 
      if(res != CURLE_OK)
         fprintf(stderr, "curl_easy_perform() failed: %s\n",
      curl_easy_strerror(res));
      curl_easy_cleanup(curl);
    }
    std::cout<<"send url complete\n";
}

void CDRManager::sendRequest(string url)
{
    try{
	
	
	url = "params="+url;
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "POST " << baseUrl << " HTTP/1.0\r\n";
        request_stream << "Host: " << server << "\r\n";
        request_stream << "User-Agent: C/1.0\r\n";
        request_stream << "Content-Type: application/x-www-form-urlencoded\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n";
        request_stream << "Content-Length: "<< url.length()<< "\r\n\r\n";
    //    request_stream << "Connection: close\r\n\r\n";
        request_stream <<url << "\r\n";
    //    boost::asio::ip::tcp::socket sock(io);
        
        
        
        
        
        boost::asio::ip::tcp::socket sock(io);
	sock.connect(ep);
        boost::system::error_code ec;
        boost::asio::write(sock,request);
        
        boost::shared_ptr<boost::asio::streambuf> buf = boost::shared_ptr<boost::asio::streambuf>(new boost::asio::streambuf());
	boost::asio::async_read_until(sock,*buf,"\r\n\r\n",boost::bind(&CDRManager::read_handler,this,buf,_1,_2));
	sock.close();
    }
    catch (exception& e)
    {
        cout<<"CATCH EXCEPTION!!!" << e.what() << '\n';
    }
    
    return;


}

void CDRManager::read_handler(boost::shared_ptr<boost::asio::streambuf> databuf,const boost::system::error_code& ec,std::size_t size)
{
    try{
	if (!ec)
	{
    	    boost::asio::streambuf& buf = *databuf;
    	    string str(boost::asio::buffers_begin(buf.data()), boost::asio::buffers_begin(buf.data()) + buf.size());
    
	    buf.consume(size);
    	    lm.makeLog(info,"[CDRManager]:\n"+str);
	    std::cout<<str<<"\n";
	}
	else
	{
	    std::cout<<ec.category().name() << ':' << ec.value();
    	    string errmsg = ec.category().name() + ec.value();
    	    lm.makeLog(boost::log::trivial::severity_level::error,"NetWork ERROR: "+errmsg);
	    
	}
    }
    catch(exception &ec)
    {
        string errmsg = "Error in CDRManager::read_handle ";
        lm.makeLog(boost::log::trivial::severity_level::error,errmsg+ec.what());
        std::cout<<"ERROR CDRMAnager read_handler "<<errmsg<<" "<<ec.what()<<"\n";
    }
    
}
                    

std::string CDRManager::map2json (const std::map<std::string, std::string>& map) 
{
    //using boost::property_tree::ptree;
    
    //ptree pt;
    string buf = "{"; 
    for (auto& entry: map) 
    {
	std::cout<<"json decode "<<entry.first<<"  "<<entry.second<<"\n";
	//pt.put (entry.first, entry.second);
	
	buf+="\"";
	buf+=entry.first;
	if(entry.second!="NULL")
	{   buf+="\":\"";
	    buf+=entry.second;
	    buf+="\",";
	}
	else
	{
	    buf+="\":null,";
	}
    }
    buf.pop_back();
    buf+="}";
    //std::ostringstream buf; 
    //write_json (buf, pt, false); 
    return buf;//.str();
}