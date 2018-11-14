// Joe Meyer and Ezra Schwaartz

#include "server.cc"

using namespace Pistache;
using namespace Pistache::Http;
using namespace std;
using namespace server;


void create_cache(Address server_name, Port port, Cache::index_type memSize) {

	StatsEndpoint stats(addr);

	cout << "Server up and running..." << endl;

	stats.init(thread, memSize);
	stats.start();

	stats.shutdown();
}

Cache::index_type space_used_test(port) {
	Cache::index_type memused = PUT localhost:port/"memused";
	return memused;
}


// sets Cache[key] = val
void set_test(Cache* c, Cache::key_type key, int* val, Cache::index_type size, port) {
	cout << "setting Cache['" << key << "'] = " << *val << "\n";
	SET 
}




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

	create_cache(addr, port, memSize);

}









// sets Cache[key] = val
void set_test(Cache* c, Cache::key_type key, int* val, Cache::index_type size, port) {
	cout << "setting Cache['" << key << "'] = " << *val << "\n";
	server
}


// prints value at key
void get_test(Cache* c, Cache::key_type key, Cache::index_type& val_size) {
	cout << "getting Cache['" << key << "']: ";
	Cache::val_type value = c->get(key, val_size);
	int* data_at_val = new int[1];
	if(value!=nullptr) {
		memcpy(data_at_val, value, val_size);
		cout << *data_at_val << '\n';
	}
	free(data_at_val);
}

// deletes Cache[key]
void del_test(Cache* c, Cache::key_type key) {
	cout << "deleting Cache['" << key << "']\n";
	c->del(key);
}


// this is a functor
class betterHasher {
private:
	hash<string> hasher_;
	int bound_;

public:
	// hashes key to int in range(0, bound)
	uint32_t operator()(string key) {
		return this->hasher_(key)%this->bound_;
	}
	betterHasher(int bound);
};

betterHasher::betterHasher(int bound) {
	this->bound_ = bound;
}





