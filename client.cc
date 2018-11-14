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









Cache::index_type space_used_test(Cache* c) {
	Cache::index_type used_space = c->space_used();
	cout << "space used: " << used_space << "\n";
	PUT localhost
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




int main() {
	// initialize Cache obj 'cache_length'
	uint32_t cache_length = 2;

	cout << "Initializing myHasher with bound 2 [val.s should hash to 1 or 0]...\n";

	betterHasher myHasher = betterHasher(cache_length);

	//test hasher
	cout << "'key' hashes to " << myHasher("key") << '\n';
	cout << "'ckey3' hashes to " << myHasher("ckey3") << "\n\n";

	cout << "Creating cache using myHasher w maxmem 2...\n";
	cout << "(values by default are 4 bytes long)\n";

	// size is size of vals (which are ints)
	uint32_t size = sizeof(uint32_t);


	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);

	assert(space_used_test(myCache)==0 && "empty cache should have no elements");


	int x = 21;
	set_test(myCache, "key", &x, size);
	assert(space_used_test(myCache)==size && "space used after 1 insert should be 4 bytes");
	// get present element
	get_test(myCache, "key", size);
	assert(space_used_test(myCache)==size && "'get' should not affect space used");
	cout << '\n';

	// overwrite present element
	x = 16;
	set_test(myCache, "key", &x, size);
	// see if it overwrote
	get_test(myCache, "key", size);
	//size should still be 1
	assert(space_used_test(myCache)==size && "overwrite should not affect space used");
	cout << '\n';

	//test get/del an absent element
	get_test(myCache, "keyAbsent", size);
	del_test(myCache, "keyAbsent");
	assert(space_used_test(myCache)==size && "del.ing absent el. should not affect space used");

	cout << '\n';
	//test del present element
	get_test(myCache, "key", size);
	del_test(myCache, "key");
	//see if del worked
	assert(space_used_test(myCache)==0 && "del.ing present el. should decrement space used");
	get_test(myCache, "key", size);
	

	//test evict (add 3 el.s, check that 1 gets evicted) [all same size => tests FIFO-evict]
	cout << "\ntesting Fifo-evict, adding 3 same-sized el.s to a map w max_load 2...\n";
	x=17;
	set_test(myCache, "key", &x, size);
	x = 18;
	set_test(myCache, "key1", &x, size);
	cout << "should evict 'key' next since all same size vals => FIFO\n";
	x = 19;
	set_test(myCache, "key2", &x, size);
	// size should be 2 not 3
	cout << "map size should be 2*4 = 8: ";
	assert(space_used_test(myCache)==2*size && "Should add 3, evict 1");
	cout << '\n';

	get_test(myCache, "key", size);
	get_test(myCache, "key1", size);
	get_test(myCache, "key2", size);
	cout << '\n';
	del_test(myCache, "key1");
	del_test(myCache, "key2");	
	space_used_test(myCache);
	cout << '\n';

	// tests set, LRU evict, and overwrite (including overwriting dif. size)
	cout << "testing LRU-evict, overwrite: inserting 4 el.s in ascending size\n";
	cout << "(1 should get evicted and 1 overwritten)\n";
	cout << "(the first el. is 4 bytes and each el. is 1 byte longer than the last)\n";
	x=31;
	set_test(myCache, "ckey1", &x, size);
	x = 32;
	set_test(myCache, "ckey2", &x, size+1);
	x = 33;

	cout << "'ckey2' should get evicted next bc it's biggest\n";
	set_test(myCache, "ckey3", &x, size+2);
	x = 34;
	cout << "overwriting 'ckey3' with dif. sized value\n";
	set_test(myCache, "ckey3", &x, size+3);

	cout << "space used should be 4 + (4+3) = 11: ";
	assert(space_used_test(myCache)==(size + (size + 3)) && "Should add 3, evict 1, overwrite 1");
	cout << '\n';

	get_test(myCache, "ckey1", size);
	get_test(myCache, "ckey2", size);
	get_test(myCache, "ckey3", size);

	free(myCache);

}

