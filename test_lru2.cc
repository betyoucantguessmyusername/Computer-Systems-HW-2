// Joe Meyer

#include "cache_lru.cc"

using namespace std;


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


// prints/returns value at key
int get_interface(Cache* c, Cache::key_type key, Cache::index_type& val_size) {
	Cache::val_type value = c->get(key, val_size);
	int* data_at_val = new int[1];
	// data is hard copy (int) of int* data_at_val
	int data;
	if(value!=nullptr) {
		memcpy(data_at_val, value, val_size);
		data = *data_at_val;
	}
	free(data_at_val);
	return data;
}

// ensure hash-values are consistent
void test_hasher() {
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);

	uint32_t keyhash = myHasher("key");
	uint32_t ckey3hash = myHasher("ckey3");
	uint32_t newkeyhash = myHasher("key");
	uint32_t newckey3hash = myHasher("ckey3");
	assert(newkeyhash==keyhash);
	assert(newckey3hash==ckey3hash);

}


// insert item into cache, query it, assert both value is unchanged
void test_set_insert() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);
	uint32_t first_val = 1;
	Cache::key_type key = "key";

	// insert item
	myCache->set(key, &first_val, size);

	// query item and assert value is correct
	assert(get_interface(myCache, "key", size) == 1 && "'key' value is wrong");
}

// fill cache, do insert and query it
void test_set_insert_full() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);
}

// set element, overwrite it with different size, query it
void test_set_overwrite() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);
}


void test_evictor_fifo() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);
}


void test_evictor_lru() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);
}


void test_get_present() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);
}


void test_get_absent() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);
}


void test_get_deleted() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);
}


void test_delete_present() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);

	int val = 42;
	Cache::key_type key = "key";
	Cache::index_type val_size = sizeof(uint32_t);
	myCache->set(key, &val, val_size);
	myCache->del(key);
	assert(myCache->space_used() == 0 && "Space should be empty since key was deleted.");
}


void test_delete_absent() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);

	Cache::key_type key = "key";
	uint32_t val_size = sizeof(uint32_t);

	Cache::index_type space_used = myCache->space_used();
	myCache->del(key);
	assert(space_used == myCache->space_used() && "Deleting an absent key doesn't impact the Cache.");
}


void test_space_used() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);

	Cache::index_type space_used = myCache->space_used();
	assert(space_used == 0 && "Cache should initially be empty");
}

// insert element, check space used
void test_space_used_insert() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);

	int val = 42;
	Cache::key_type key = "key";
	uint32_t val_size = sizeof(uint32_t);
	myCache->set(key, &val, val_size);
	Cache::index_type space_used = myCache->space_used();
	assert(space_used == val_size && "Space used should be the size.");
}

// insert element, check space used, delete it, check again
void test_space_used_delete() {
	uint32_t cache_length = 2;
	uint32_t size = sizeof(uint32_t);
	uint32_t bound = 2;
	betterHasher myHasher = betterHasher(bound);
	Cache* myCache = new Cache(cache_length*size, [](){ return 0; }, myHasher);

	int val = 42;
	Cache::key_type key = "key";
	uint32_t val_size = sizeof(uint32_t);
	myCache->set(key, &val, val_size);
	myCache->del(key);
	Cache::index_type space_used = myCache->space_used();
	assert(space_used == 0 && "Space used should empty now that we've deleted the only element.");
}


int main(){
	cout << "Running test_set_insert() \t\t"; 
	test_set_insert();
	cout << "PASS" << endl;

	cout << "Running test_delete_present() \t\t"; 
	test_delete_present();
	cout << "PASS" << endl;

	cout << "Running test_delete_absent() \t\t"; 
	test_delete_absent();
	cout << "PASS" << endl;

	cout << "Running test_space_used() \t\t"; 
	test_space_used();
	cout << "PASS" << endl;

	cout << "Running test_space_used_insert() \t"; 
	test_space_used_insert();
	cout << "PASS" << endl;

	cout << "Running test_space_used_delete() \t"; 
	test_space_used_delete();
	cout << "PASS" << endl;
}






// int main() {
	/*
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
	get_interface(myCache, "key", size);
	assert(space_used_test(myCache)==size && "'get' should not affect space used");
	cout << '\n';

	// overwrite present element
	x = 16;
	set_test(myCache, "key", &x, size);
	// see if it overwrote
	get_interface(myCache, "key", size);
	//size should still be 1
	assert(space_used_test(myCache)==size && "overwrite should not affect space used");
	cout << '\n';

	//test get/del an absent element
	get_interface(myCache, "keyAbsent", size);
	del_test(myCache, "keyAbsent");
	assert(space_used_test(myCache)==size && "del.ing absent el. should not affect space used");

	cout << '\n';
	//test del present element
	get_interface(myCache, "key", size);
	del_test(myCache, "key");
	//see if del worked
	assert(space_used_test(myCache)==0 && "del.ing present el. should decrement space used");
	get_interface(myCache, "key", size);
	

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

	get_interface(myCache, "key", size);
	get_interface(myCache, "key1", size);
	get_interface(myCache, "key2", size);
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

	get_interface(myCache, "ckey1", size);
	get_interface(myCache, "ckey2", size);
	get_interface(myCache, "ckey3", size);

	free(myCache);
	*/

// }

