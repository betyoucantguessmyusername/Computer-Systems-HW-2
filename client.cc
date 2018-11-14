/* 
   Mathieu Stefani, 07 février 2016
   
 * Http client example
*/

#include <atomic>

#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/client.h>
#include "cache.hh"

using namespace Pistache;
using namespace Pistache::Http;
using namespace std;
using namespace server;

class client:
private:
	Cache* cache_;
public:
	void makecache() {
		cache_();
	}

	void shutdown() {
		~cache_();
		free(cache_);
	}

	Cache::index_type space_used() {
		return cache_.space_used();
	}

	int del(Cache::key_type key){
		return cache_.del(key);
	}

	Cache::val_type get(key_type key, index_type size){
		return cache_.get(key,size);
	}

	int set(key_type key, val_type val, index_type size) {
		return cache_.set(key,val,size);
	}



main(int argc, char argv[])
{
	if (argc==2) {
		if argv[1] == "shutdown" {
			shutdown();
		} else if argv[1] == "memsize" { //Should this be space_used? since the terminal is calling it like a cache? idk
			cout << space_used();
		} else {
			del(argv[1]);	
		}
	} else if (argc == 3) {
		get(argv[1],argv[2]);
	} else if (argc == 4) {
		set(argv[1],argv[2],argv[3]);
	} else {
		cout << "wrong temrinal line input " << endl;
	}
}
