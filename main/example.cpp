#include <iostream>

#include "test/test.h"

int main()
{
    std::boolalpha(std::cout);
#define ARRAY_DEMO 0
#define MAP_DEMO 0
#define STRING_DEMO 1
#define ASYNC_DEMO 0
#if ARRAY_DEMO
    test::demo_Array();
#endif // ARRAY_DEMO
#if MAP_DEMO
    test::demo_Map();
#endif // MAP_DEMO
#if STRING_DEMO
    test::demo_String();
#endif // STRING_DEMO
#if ASYNC_DEMO
    test::demo_async();
#endif // ASYNC_DEMO
    return 0;
}