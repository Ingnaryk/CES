#include "../inc/Test.h"

int main()
{
     std::boolalpha(std::cout);
#define ARRAY_TEST 1
#define ASYNC_TEST 0
#define STRING_TEST 0
#if ARRAY_TEST
     Test::testArray();
#endif // ARRAY_TEST
#if ASYNC_TEST
     Test::testAsync();
#endif // ASYNC_TEST
#if STRING_TEST
     Test::testString();
#endif // STRING_TEST
     return 0;
}