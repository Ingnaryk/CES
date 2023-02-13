#include <iostream>

#include "test.h"

int main()
{
     std::boolalpha(std::cout);
#define ARRAY_TEST 1
#define ASYNC_TEST 0
#define STRING_TEST 0
#if ARRAY_TEST
     test::testArray();
#endif // ARRAY_TEST
#if ASYNC_TEST
     test::testAsync();
#endif // ASYNC_TEST
#if STRING_TEST
     test::testString();
#endif // STRING_TEST
     return 0;
}