#include "../inc/CArray.h"
#include "../inc/JSArray.h"
#include "../inc/Async.h"
#include <chrono>
#include <random>
using namespace std;

template <typename T>
void test(T &arr)
{
     cout << "display arr\n"
          << arr << endl;
     //------------------------push
     int num = 13;
     cout << "int num = 13, arr.push(&num) returns " << arr.push(&num) << ", and arr is\n"
          << arr << endl;
     //------------------------unshift
     cout << "arr.unshift(-6, 2147483648) returns " << arr.unshift(-6, 2147483648) << ", and arr is\n"
          << arr << endl;

     //------------------------pop
     cout << "arr.pop() returns " << arr.pop() << ", and arr is\n"
          << arr << endl;
     //------------------------shift
     cout << "arr.shift() returns " << arr.shift() << ", and arr is\n"
          << arr << endl;
     //------------------------at
     cout << "arr.at(2) returns " << arr.at(2) << ", and arr.at(-1) returns " << arr.at(-1) << endl;
     //------------------------indexOf
     cout << "arr.indexOf(2147483648) returns " << arr.indexOf(2147483648) << endl;
     //------------------------lastIndexOf
     cout << "arr.lastIndexOf(2147483648) returns " << arr.lastIndexOf(2147483648) << endl;
     //------------------------incldues
     cout << "'8 is the element of arr' is " << arr.includes(8) << endl;

     //------------------------join
     cout << "arr.join() is\n\n"
          << arr.join() << "\n\na.join(\" + \") is\n\n"
          << arr.join(" + ") << "\n"
          << endl;
     //------------------------forEach & operator=
     arr = {1, 5, 3, 7, 8, 9};
     cout << "arr = {1, 5, 3, 7, 8, 9} and the square of each value is\n\n";
     arr.forEach([](Any value)
                 { cout << get<int>(value) * get<int>(value) << " "; });
     cout << "\n"
          << endl;
     //------------------------some
     cout << "'arr includes even number' is " << arr.some([](Any value)
                                                          { return get<int>(value) % 2 == 0; })
          << endl;
     //------------------------every
     cout << "'all the numbers in arr are positive' is " << arr.every([](Any value)
                                                                      { return get<int>(value) > 0; })
          << endl;
     //------------------------find
     cout << "the first number in arr which is larger than 5 is " << arr.find([](Any value)
                                                                              { return get<int>(value) > 5; })
          << endl;
     //------------------------findLast
     cout << "the last number in arr which is smaller than 9 is " << arr.findLast([](Any value)
                                                                                  { return get<int>(value) < 9; })
          << endl;
     //------------------------findIndex
     cout << "the index of first number in arr which is larger than 5 is " << arr.findIndex([](Any value)
                                                                                            { return get<int>(value) > 5; })
          << endl;
     //------------------------findLastIndex
     cout << "the index of last number in arr which is smaller than 9 is " << arr.findLastIndex([](Any value)
                                                                                                { return get<int>(value) < 9; })
          << endl;
     //------------------------reduce
     cout << "the sum of numbers in arr(with initial value 100) is " << arr.reduce([](Any arr, Any b)
                                                                                   { return get<int>(arr) + get<int>(b); },
                                                                                   100)
          << endl;
     //------------------------reduceRight
     cout << "the product of numbers in arr meanwhile adding their index(from right side) is " << arr.reduceRight([](Any arr, Any b, size_t crtIndex)
                                                                                                                  { Any r = get<int>(arr) * get<int>(b);
                                                                                                              return get<int>(r) += crtIndex; })
          << endl;
     //------------------------filter
     cout << "the array that contains the numbers larger than 3 in arr\n"
          << arr.filter([](Any value)
                        { return get<int>(value) > 3; })
          << endl;
     //------------------------map
     cout << "the array that equals each number in arr multiplied by 0.3\n"
          << arr.map([](Any value)
                     { return (double)(get<int>(value) * 0.3); })
          << endl;
     //------------------------concat
     cout << "the array that euquals arr concated with {\"str\", 0.5, false}\n"
          << arr.concat({"str", 0.5, false}) << endl;
     //------------------------slice
     cout << "the array that equals the slice of arr within index [1, 4)\n"
          << arr.slice(1, 4) << endl;
     //------------------------splice
     cout << "slice arr from index 1, delete two elements and splice with {-1, -2, -3} returns\n"
          << arr.splice(1, 2, {-1, -2, -3})
          << "\nand arr is\n"
          << arr << endl;
     //------------------------reverse
     cout << "reverse arr\n"
          << arr.reverse() << endl;
     //------------------------sort
     cout << "sort arr by default order\n"
          << arr.sort() << "\nand sort arr by descending order\n"
          << arr.sort([](Any arr, Any b)
                      { return arr > b ? -1 : 1; })
          << endl;
     //------------------------copyWithin
     cout << "copy the numbers in arr within index [4, 6) to target index 0\n"
          << arr.copyWithin(0, 4, 6) << endl;
     //------------------------fill
     cout << "fill arr with value 10 within index [0, 3)\n"
          << arr.fill(10, 0, 3) << endl;
     //------------------------operator[]
     arr[9] = -5;
     cout << "arr[9] = -5, and now arr is\n"
          << arr << endl;
}

int main()
{
     cout << boolalpha;
     /*//------------------------JSArray/CArray<T> Test------------------------//
     using milisecond = chrono::duration<double, milli>;
     auto ct = chrono::steady_clock::now();
     {
          JSArray arr = {3.5, true, 2147483648, nullptr, "this is string"};
          test(arr);
     }
     milisecond t1 = (chrono::steady_clock::now() - ct);
     cout << "----------------------------------------------------------" << endl;
     ct = chrono::steady_clock::now();
     {

          CArray<Any> arr = {3.5, true, 2147483648, nullptr, "this is string"};
          test(arr);
     }
     milisecond t2 = (chrono::steady_clock::now() - ct);
     cout << "t1: " << t1 << ", t2: " << t2 << endl;*/
     //------------------------async, await, Promise Test------------------------//
     AsyncFunction f([&](Any data)
                     { return Promise([&](ResExecutor res)
                                      { this_thread::sleep_for(1000ms);
                                      res("sizeof data is" + sizeof(data)); }) });
     return 0;
}
