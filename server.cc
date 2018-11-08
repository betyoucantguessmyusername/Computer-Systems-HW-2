//Ezra Schwartz and Joe Meyer

//#include <pistache>  
//need specific file
#include <pthread.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cache_lru.cc"

using namespace std;

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

int 
main(int argc, char **argv)
{
	//argc = argument count
	//argv = array of arguments 

	Cache::index_type memSize = 100;
	int portNum = 8080;
	int opt;

	//https://linux.die.net/man/3/getopt
	while ((opt = getopt (argc,argv, "mt:")) != -1)
		switch( opt )
			{
			case 'm':
				memSize = atoi(optarg);
				break;
			case 't':
				portNum = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s [-m maxmem] [-t portNum]\n", argv[0]);
            	exit(EXIT_FAILURE);
			}

	cout << memSize << " " << portNum << endl;

	//Initializing cache variables
	uint32_t bound = 100;
	uint32_t size = sizeof(uint32_t);
	betterHasher myHasher = betterHasher(bound);

	Cache* c = new Cache(memSize*size, myHasher);
	free(c);

}