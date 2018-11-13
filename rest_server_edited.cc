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

void printCookies(const Http::Request& req) {
	auto cookies = req.cookies();
	std::cout << "Cookies: [" << std::endl;
	const std::string indent(4, ' ');
	for (const auto& c: cookies) {
		std::cout << indent << c.name << " = " << c.value << std::endl;
	}
	std::cout << "]" << std::endl;
}

namespace Generic {

void handleReady(const Rest::Request&, Http::ResponseWriter response) {
	response.send(Http::Code::Ok, "1");
}

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

		Routes::Post(router, "/set/:key/:value/:size", Routes::bind(&StatsEndpoint::set, this));
		Routes::Get(router, "/get/:key/:valsize", Routes::bind(&StatsEndpoint::get, this));
		Routes::Get(router, "/del/:key", Routes::bind(&StatsEndpoint::del, this));


		Routes::Get(router, "/value/:name", Routes::bind(&StatsEndpoint::doGetMetric, this));
		Routes::Get(router, "/ready", Routes::bind(&Generic::handleReady));
		Routes::Get(router, "/auth", Routes::bind(&StatsEndpoint::doAuth, this));

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

	void doGetMetric(const Rest::Request& request, Http::ResponseWriter response) {
		auto name = request.param(":name").as<std::string>();

		Guard guard(metricsLock);
		auto it = std::find_if(metrics.begin(), metrics.end(), [&](const Metric& metric) {
			return metric.name() == name;
		});

		if (it == std::end(metrics)) {
			response.send(Http::Code::Not_Found, "Metric does not exist");
		} else {
			const auto& metric = *it;
			response.send(Http::Code::Ok, std::to_string(metric.value()));
		}

	}

	void doAuth(const Rest::Request& request, Http::ResponseWriter response) {
		printCookies(request);
		response.cookies()
			.add(Http::Cookie("lang", "en-US"));
		response.send(Http::Code::Ok);
	}

	class Metric {
	public:
		Metric(std::string name, int initialValue = 1)
			: name_(std::move(name))
			, value_(initialValue)
		{ }

		int incr(int n = 1) {
			int old = value_;
			value_ += n;
			return old;
		}

		int value() const {
			return value_;
		}

		std::string name() const {
			return name_;
		}
	private:
		std::string name_;
		int value_;
	};

	typedef std::mutex Lock;
	typedef std::lock_guard<Lock> Guard;
	Lock metricsLock;
	std::vector<Metric> metrics;

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
