#include "../inc/ArrayLike.h"
#include "../inc/Array.h"
#include "../inc/Object.h"
#include <time.h>
using namespace std;

/* Using C++ standard to implement major of ES6 standard features
** While coding these codes I'v learnt new C++(11~20) featrues like std::variant, std::initializer_list,
** std::fucntion, std::optional, remove_pointer, variadic templates and so on.
**
** Because of the strongly typed featre of C++,
** I suppose that some functions like valueOf, Array.of, Array.from, etc can be substituted by the constructer.
** The assignment between objects like Array is deep copy.
** Type 'size_t' used to describe length of my array, which can hold more capacity than javascript
*/

template <typename T>
void test(T &a)
{
     cout << "display a\n"
          << a << endl;
     //------------------------push
     int num = 13;
     cout << "int num = 13, a.push(&num) returns " << a.push(&num) << ", and a is\n"
          << a << endl;
     //------------------------unshift
     cout << "a.unshift(-6, 2147483648) returns " << a.unshift(-6, 2147483648) << ", and a is\n"
          << a << endl;

     //------------------------pop
     cout << "a.pop() returns " << a.pop() << ", and a is\n"
          << a << endl;
     //------------------------shift
     cout << "a.shift() returns " << a.shift() << ", and a is\n"
          << a << endl;
     //------------------------at
     cout << "a.at(2) returns " << a.at(2) << ", and a.at(-1) returns " << a.at(-1) << endl;
     //------------------------indexOf
     cout << "a.indexOf(2147483648) returns " << a.indexOf(2147483648) << endl;
     //------------------------lastIndexOf
     cout << "a.lastIndexOf(2147483648) returns " << a.lastIndexOf(2147483648) << endl;
     //------------------------incldues
     cout << "'8 is the element of a' is " << a.includes(8) << endl;

     //------------------------join
     cout << "a.join() is\n\n"
          << a.join() << "\n\na.join(\" + \") is\n\n"
          << a.join(" + ") << "\n"
          << endl;
     //------------------------forEach & operator=
     a = {1, 5, 3, 7, 8, 9};
     cout << "a = {1, 5, 3, 7, 8, 9} and the square of each value is\n\n";
     a.forEach([](Any value, ...)
               { cout << get<int>(value) * get<int>(value) << " "; });
     cout << "\n"
          << endl;
     //------------------------some
     cout << "'a includes even number' is " << a.some([](Any value, ...)
                                                      { return get<int>(value) % 2 == 0; })
          << endl;
     //------------------------every
     cout << "'all the numbers in a are positive' is " << a.every([](Any value, ...)
                                                                  { return get<int>(value) > 0; })
          << endl;
     //------------------------find
     cout << "the first number in a which is larger than 5 is " << a.find([](Any value, ...)
                                                                          { return get<int>(value) > 5; })
          << endl;
     //------------------------findLast
     cout << "the last number in a which is smaller than 9 is " << a.findLast([](Any value, ...)
                                                                              { return get<int>(value) < 9; })
          << endl;
     //------------------------findIndex
     cout << "the index of first number in a which is larger than 5 is " << a.findIndex([](Any value, ...)
                                                                                        { return get<int>(value) > 5; })
          << endl;
     //------------------------findLastIndex
     cout << "the index of last number in a which is smaller than 9 is " << a.findLastIndex([](Any value, ...)
                                                                                            { return get<int>(value) < 9; })
          << endl;
     //------------------------reduce
     cout << "the sum of numbers in a(with initial value 100) is " << a.reduce([](Any a, Any b, ...)
                                                                               { return get<int>(a) + get<int>(b); },
                                                                               100)
          << endl;
     //------------------------reduceRight
     cout << "the product of numbers in a meanwhile adding their index(from right side) is " << a.reduceRight([](Any a, Any b, size_t crtIndex, ...)
                                                                                                              { Any r = get<int>(a) * get<int>(b);
                                                                                                              return get<int>(r) += crtIndex; })
          << endl;
     //------------------------filter
     cout << "the array that contains the numbers larger than 3 in a\n"
          << a.filter([](Any value, ...)
                      { return get<int>(value) > 3; })
          << endl;
     //------------------------map
     cout << "the array that equals each number in a multiplied by 0.3\n"
          << a.map([](Any value, ...)
                   { return (double)(get<int>(value) * 0.3); })
          << endl;
     //------------------------concat
     cout << "the array that euquals a concated with {\"str\", 0.5, false}\n"
          << a.concat({"str", 0.5, false}) << endl;
     //------------------------slice
     cout << "the array that equals the slice of a within index [1, 4)\n"
          << a.slice(1, 4) << endl;
     //------------------------splice
     cout << "slice a from index 1, delete two elements and splice with {-1, -2, -3} returns\n"
          << a.splice(1, 2, {-1, -2, -3})
          << "\nand a is\n"
          << a << endl;
     //------------------------reverse
     cout << "reverse a\n"
          << a.reverse() << endl;
     //------------------------sort
     cout << "sort a by default order\n"
          << a.sort() << "\nand sort a by descending order\n"
          << a.sort([](Any a, Any b)
                    { return a > b ? -1 : 1; })
          << endl;
     //------------------------copyWithin
     cout << "copy the numbers in a within index [4, 6) to target index 0\n"
          << a.copyWithin(0, 4, 6) << endl;
     //------------------------fill
     cout << "fill a with value 10 within index [0, 3)\n"
          << a.fill(10, 0, 3) << endl;
     //------------------------operator[]
     a[9] = -5;
     cout << "a[9] = -5, and now a is\n"
          << a << endl;
}

int main()
{
     cout << boolalpha;
     //------------------------Object Test------------------------//
     //------------------------operator[]
     Object o;
     o[3] = "string 7";
     o["3"] = 7;
     o["1"] = false;
     o["string key"] = 10.2;
     o[-3] = nullptr;
     o["what"] = -9;
     cout << o << endl;
     //------------------------_delete
     o._delete("inexistent key");
     o._delete("what");
     cout << o << endl;
     //------------------------keys&values
     cout << "keys of o\n"
          << Object::keys(o) << endl;
     cout << "values of o\n"
          << Object::values(o) << endl;
     /*//------------------------Array/ArrayLike<T> Test------------------------//
     clock_t st = clock();
     {
          Array a = {3.5, true, 2147483648, nullptr, "this is a string"};
          test(a);
     }
     clock_t t1 = clock() - st;
     cout << "----------------------------------------------------------" << endl;
     st = clock();
     {
          ArrayLike<Any> a = {3.5, true, 2147483648, nullptr, "this is a string"};
          test(a);
     }
     clock_t t2 = clock() - st;
     cout << "t1: " << t1 << ", t2: " << t2 << endl;*/
     return 0;
}