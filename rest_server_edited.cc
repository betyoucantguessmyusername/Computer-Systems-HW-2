/* 
   Mathieu Stefani, 07 février 2016
   
   Example of a REST endpoint with routing


   Modified by Ezra Schwartz and Joe Meyer
*/

#include <algorithm>

#include <pistache/http.h>
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

	void init(size_t thr = 2) {
		auto opts = Http::Endpoint::options()
			.threads(thr)
			.flags(Tcp::Options::InstallSignalHandler);
		httpEndpoint->init(opts);
		setupRoutes();

		cache_ = makeCache();



	}

	Cache* makeCache(uint32_t memSize = 2) {
		//Initializing cache variables
		uint32_t bound = 100;
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

		Routes::Put(router, "/set/:key/:value/:size", Routes::bind(&StatsEndpoint::set, this));
		Routes::Get(router, "/get/:key/:valsize", Routes::bind(&StatsEndpoint::get, this));
		Routes::Get(router, "/del/:key", Routes::bind(&StatsEndpoint::del, this));



	}

	void set(const Rest::Request& request, Http::ResponseWriter response) {
		auto key = request.param(":key").as<std::string>();
		auto value = request.param(":value").as<std::string>();
		auto size_string = request.param(":size").as<std::string>();
		uint32_t size = atoi(size_string.c_str());

		int status = cache_->set(key, &value, size);
		response.send(Http::Code::Ok, std::to_string(status));

	}

	void get(const Rest::Request& request, Http::ResponseWriter response) {
		auto key = request.param(":key").as<std::string>();
		auto valsize_string = request.param(":valsize").as<std::string>();
		uint32_t valsize = atoi(valsize_string.c_str());

		Cache::val_type status = cache_->get(key, valsize);
		int* status_nonvoid = new int[1];
		if(status!= nullptr) {
			memcpy(status_nonvoid, status, valsize);
		}
		response.send(Http::Code::Ok, std::to_string(*status_nonvoid));

	}

	void del(const Rest::Request& request, Http::ResponseWriter response) {
		auto key = request.param(":key").as<std::string>();

		int status = cache_->del(key);
		response.send(Http::Code::Ok, std::to_string(status));

	}



	std::shared_ptr<Http::Endpoint> httpEndpoint;
	Rest::Router router;
};

int main(int argc, char *argv[]) {
	Port port(9080);

	int thr = 2;

	if (argc >= 2) {
		port = std::stol(argv[1]);

		if (argc == 3)
			thr = std::stol(argv[2]);
	}

	Address addr(Ipv4::any(), port);

	cout << "Cores = " << hardware_concurrency() << endl;
	cout << "Using " << thr << " threads" << endl;

	StatsEndpoint stats(addr);

	stats.init(thr);
	stats.start();

	stats.shutdown();
}
