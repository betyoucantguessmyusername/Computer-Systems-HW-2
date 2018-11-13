/* 
   Mathieu Stefani, 07 février 2016
   
   Example of a REST endpoint with routing


   Modified by Ezra Schwartz and Joe Meyer
*/

#include <algorithm>
#include <unistd.h>
#include <pistache/http.h>
#include <pistache/http_headers.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include "cache_lru.cc"

using namespace std;
using namespace Pistache;

// this is a functor
class betterHasher {
private:
	hash<string> hasher_;
	uint32_t bound_;

public:
	// hashes key to uint32_t in range(0, bound)
	uint32_t operator()(string key) {
		return this->hasher_(key)%this->bound_;
	}
	betterHasher(uint32_t bound);
};

betterHasher::betterHasher(uint32_t bound) {
	this->bound_ = bound;
}

class StatsEndpoint {
public:
	StatsEndpoint(Address addr)
		: httpEndpoint(std::make_shared<Http::Endpoint>(addr))
	{ }

	Cache* cache_;

	void init(size_t thr = 2, Cache::index_type memSize = 100) {
		auto opts = Http::Endpoint::options()
			.threads(thr)
			.flags(Tcp::Options::InstallSignalHandler);
		httpEndpoint->init(opts);
		setupRoutes();

		cache_ = makeCache(memSize);
	}

	Cache* makeCache(uint32_t memSize = 2) {
		//Initializing cache variables
		uint32_t bound = memSize;
		uint32_t size = sizeof(uint32_t);
		betterHasher myHasher = betterHasher(bound);

		return new Cache(memSize*size, myHasher);
	}

	void start() {
		httpEndpoint->setHandler(router.handler());
		httpEndpoint->serve();
	}

	void shutdown() {
		httpEndpoint->shutdown();
	}

private:
	void setupRoutes() {
		using namespace Rest;

		Routes::Get(router, "/get/:key/:valsize", Routes::bind(&StatsEndpoint::get, this));
		Routes::Get(router, "/get/memsize", Routes::bind(&StatsEndpoint::get, this));
		Routes::Put(router, "/set/:key/:value/:size", Routes::bind(&StatsEndpoint::set, this));
		Routes::Delete(router, "/del/:key", Routes::bind(&StatsEndpoint::del, this));
		Routes::Post(router, "/shutdown", Routes::bind(&StatsEndpoint::destroy, this));
	}

	void get(const Rest::Request& request, Http::ResponseWriter response) {
		Cache::val_type getStatus;
		Cache::index_type memStatus;
		string json = "{}";

		if (request.hasParam(":key")){
			auto key = request.param(":key").as<std::string>();
			auto valsize_string = request.param(":valsize").as<std::string>();
			uint32_t valsize = atoi(valsize_string.c_str());

			getStatus = cache_->get(key, valsize);
			int* status_nonvoid = new int[1];
			if(getStatus!= nullptr) {
				memcpy(status_nonvoid, getStatus, valsize);
				json = "{ key: " + key + ", value: " + std::to_string(*status_nonvoid)+" }";
			} 
		}
		else if (request.hasParam("memsize")){
			memStatus = cache_->space_used();
			json = "{ memused: "+ to_string(memStatus)+" }";
		}	
		response.send(Http::Code::Ok, json);
	}

	void set(const Rest::Request& request, Http::ResponseWriter response) {
		auto key = request.param(":key").as<std::string>();
		auto value = request.param(":value").as<std::string>();
		auto size_string = request.param(":size").as<std::string>();
		uint32_t size = atoi(size_string.c_str());

		int status = cache_->set(key, &value, size);
		response.send(Http::Code::Ok, to_string(status));
	}

	void del(const Rest::Request& request, Http::ResponseWriter response) {
		auto key = request.param(":key").as<std::string>();

		int status = cache_->del(key);
		response.send(Http::Code::Ok, to_string(status));
	}

	// Code for exiting
	void destroy(const Rest::Request& request, Http::ResponseWriter response) {
		free(cache_);
		response.send(Http::Code::Ok, "0");
	}

	std::shared_ptr<Http::Endpoint> httpEndpoint;
	Rest::Router router;
};

int main(int argc, char *argv[]) {
	Port port(8080);
	int thr = 2;
	Cache::index_type memSize = 100;

	int opt;

	//https://linux.die.net/man/3/getopt
	while ((opt = getopt (argc,argv, "m:t:")) != -1)
		switch( opt )
			{
			case 'm':
				memSize = atoi(optarg);
				break;
			case 't':
				port = stol(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s [-m maxmem] [-t portNum]\n", argv[0]);
            	exit(EXIT_FAILURE);
			}

	Address addr(Ipv4::any(), port);

	cout << "Server up and running..." << endl;

	StatsEndpoint stats(addr);

	stats.init(thr, memSize);
	stats.start();

	stats.shutdown();
}
