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

#include "cache.hh"

using namespace std;
using namespace Pistache;
using namespace Pistache::Http;

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

		Routes::Get(router, "/:key/:valsize", Routes::bind(&StatsEndpoint::getValue, this));
		Routes::Get(router, "/memsize", Routes::bind(&StatsEndpoint::getMemsize, this));
		Routes::Put(router, "/:key/:value/:size", Routes::bind(&StatsEndpoint::set, this));
		Routes::Delete(router, "/:key", Routes::bind(&StatsEndpoint::del, this));
		Routes::Post(router, "/shutdown", Routes::bind(&StatsEndpoint::destroy, this));
	}

	void getValue(const Rest::Request& request, Http::ResponseWriter response) {
		Cache::val_type getStatus;
		string json = "{error}";

		auto key = request.param(":key").as<std::string>();
		auto valsize_string = request.param(":valsize").as<std::string>();
		uint32_t valsize = atoi(valsize_string.c_str());

		getStatus = cache_->get(key, valsize);
		int* status_nonvoid = new int[1];
		if(getStatus!= nullptr) {
			memcpy(status_nonvoid, getStatus, valsize);
			json = "{ key: " + key + ", value: " + std::to_string(*status_nonvoid)+" }";
		} 
		cout << key << valsize <<endl;
		response.send(Http::Code::Ok, json);
	}
	void getMemsize(const Rest::Request& request, Http::ResponseWriter response) {
		Cache::index_type memStatus;
		string json = "{error}";

		memStatus = cache_->space_used();
		json = "{ memused: "+ to_string(memStatus)+" }";

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
		cout << "got here" << endl;
		free(cache_);
		response.send(Http::Code::Ok, "0");
		sleep(3);
		cout << "got here again" << endl;
		shutdown();
	}

	std::shared_ptr<Http::Endpoint> httpEndpoint;
	Rest::Router router;
};

int main(int argc, char *argv[]) {

	if (argc != 3) {
        std::cerr << argc << "Usage: http_client memSize port " << std::endl;
        return 1;
    }

    Cache::index_type memSize = stoi(argv[1]);
    Port port = stoi(argv[2]);
    int thread = 1;
    Address addr(Ipv4::any(), port);
	StatsEndpoint stats(addr);

    cout << "Server up and running..." << endl;

    stats.init(thread, memSize);
    stats.start();

    exit(EXIT_SUCCESS);
}


