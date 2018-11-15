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

class myClient {
private:
	Cache* cache_;

	myClient(uint32_t cache_length, Cache::hash_func hasher) {
		cache_ = new Cache(cache_length*sizeof(uint32_t), hasher);
	};
public:

	void shutdown() {
		free(cache_);
	}

	Cache::index_type space_used() {
		return cache_->space_used();
	}

	int del(Cache::key_type key){
		return cache_->del(key);
	}

	Cache::val_type get(Cache::key_type key, Cache::index_type size){
		return cache_->get(key,size);
	}

	int set(Cache::key_type key, Cache::val_type val, Cache::index_type size) {
		return cache_->set(key,val,size);
	}

};

void myClient::myClient() {
	cache_();
}



int main(int argc, char* argv[])
{
	myClient* client = myClient();
	// args to main: client [fn args]
	if (argc==2) {
		if (argv[1] == "shutdown") {
			client->shutdown();
		} else if (argv[1] == "memsize") { //Should this be space_used? since the terminal is calling it like a cache? idk
			client->space_used();
		} else {
			client->del(argv[1]);	
		}
	} else if (argc == 3) {
		client->get(argv[1],atoi(argv[2]));
	} else if (argc == 4) {
		client->set(argv[1],argv[2],atoi(argv[3]));
	} else {
		cout << "wrong temrinal line input " << endl;
	}
}
