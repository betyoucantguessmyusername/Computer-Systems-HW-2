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

int 
main(int argc, char **argv)
{
	//argc = argument count
	//argv = array of arguments 

	int mFlag = 0;
	int tFlag = 0;
	int index = 0;
	int index;
	int c;

	//https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
	while ((c = getopt (argc,argv, "abc:")) != -1)
		switch( c )
			{
			case 'm':
				mFlag = 1;
				break;
			case 't':
				tFlag = 1;
				break;
			}

	opterr = 0;

	// assert (argc == 2 && "You have to enter two arguments, maxmem and portnum");
	// auto maxmem = argv[1];
	// auto portnum = argv[2];

	//Initializing cache variables
	uint32_t size = sizeof(uint32_t);
	betterHasher myHasher = betterHasher(bound);

	Cache* c = new Cache(maxmem*size, myHasher);


}