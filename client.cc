/* 
   Mathieu Stefani, 07 février 2016
   
 * Http client example
*/

#include <atomic>

#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/client.h>
#include <cache_lru.cc>
#include "server.cc"

using namespace Pistache;
using namespace Pistache::Http;
using namespace std;
using namespace server;
using namespace cache_lru


void create_cache(Address server_name, Port port, int thread, Cache::index_type memSize) {

    StatsEndpoint stats(addr);

    cout << "Server up and running..." << endl;

    stats.init(thread, memSize);
    stats.start();

}

void get(string request) {
    auto resp = client.get(request).cookie(Http::Cookie("FOO", "bar")).send();
    resp.then([&](Http::Response response) {
        std::cout << "Response code = " << response.code() << std::endl;
        auto body = response.body();
        if (!body.empty())
           std::cout << "Response body = " << body << std::endl;
    }, Async::IgnoreException);
    responses.push_back(std::move(resp));


}

void destroy() {
    client.shutdown();
}


int main(int argc, char *argv[]) {

  



    if (argc != 2) {
        std::cerr << "Usage: http_client memSize port [threads]" << std::endl;
        return 1;
    }

    Cache::index_type memSize =  stoi(argv[1]);
    Port port = stoi(argv[2]);

    global vector<Async::Promise<Http::Response>> responses;
    global int thread = 1;

    if (argc==3) {
        thread = stoi(arv[3]);
    }

    //Question: What is server name?


    Http::Client client;

    auto opts = Http::Client::options()
        .threads(thread)
        .maxConnectionsPerHost(8);
    client.init(opts);


    Address addr(Ipv4::any(), port);

    create_cache(addr, port, memSize);




}
