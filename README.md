#Computer Systems HW 3 
Joe Meyer & Ezra Schwartz

Google sheets link to the table for Part 1 and Part 2:
https://docs.google.com/spreadsheets/d/1SGC-DK_hKgTCKbOK3QXLqUpMrs5UVX2u7eCNvrbwQXQ/edit?usp=sharing

(Only Reed College email addresses can view, email ezraschwartz@reed.edu if you have issues opening)

#Henry's Code:

When we initially run our tests on this code, we get an error:
  "cache.cpp:1:10: fatal error: cache.h: No such file or directory
   #include <cache.h>
            ^~~~~~~~~
  compilation terminated."
  
When I change this code to say "#include "cache.h" instead of "#include <cache.h>", we get an error:

  "In file included from test_final.cc:3:
  cache.cpp: In member function ‘const void* Cache::get(key_type, Cache::index_type)’:
  cache.cpp:182:1: warning: no return statement in function returning non-void [-Wreturn-type]
   }
   ^
  cache.cpp: In member function ‘Cache::index_type Cache::space_used() const’:
  cache.cpp:192:1: warning: no return statement in function returning non-void [-Wreturn-type]
   }"
   ^
  [followed by a bunch of gibberish I won't copy here, which terminates in: ]
  
  "collect2: error: ld returned 1 exit status"

Thus there appears to be at least two issues, one in the format of cache.h importation and one linker error. I noticed that the cache.cpp file (c++) imports the cache.h file (c). I don't know whether this relates to the linker error.
There also appears to be an error where a function returning a non-void value does not return any value. I cannot say anything beyond this because it doesn't compile.

#Kathy's Code:

As you can see from the table, their code passed all of the tests.  All we had to adjust when testing their code was that we included cstring and assert.  Also, they sent a message that we had to delete their main in their cache.cc, which when we did allowed for the test results.

#Sam Zofkie's Code:

We did not find any errors in Sam's code. All our tests passed. None of them failed. test_set_insert() did not fail. Neither did test_set_overwrite. Test_hasher() didn't fail either. test_space_used() failed very little, in fact not at all. Not even test-delete-absent failed. In fact, pick any small number - the number of tests that failed is less than that. So small is the number of tests that failed, that if each failed test were the size of a skyscraper you would still not be able to see any test failures.



