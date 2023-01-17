#include "../inc/Test.h"

/* simple implement of ECMAScript standard by C++
** the purpose for this repository is,
** on the one hand, to absorb modern grammer skills and thoughts
** on the other hand, to learn new C++(11~20) featrues like std::variant, std::initializer_list, std::fucntion, std::optional, remove_pointer, variadic templates and so on.
**
** because of the strongly typed featre of C++,
** The assignment between objects is kind of deep copy.
*/

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