#ifndef DBMEMCACHUTILS_HEADER
#define DBMEMCACHUTILS_HEADER 1

#include <boost/thread/mutex.hpp>
#include <string>
#include <map>
#include <vector>
#include <libmemcached/memcached.h>

using namespace std;

class DBMemcachutils
{
	memcached_st* handle;
	void put_values(const char* key,const char* value);
    public:
	DBMemcachutils();
	~DBMemcachutils();
	
};


#endif