#include "Any.h"
#include "RawArray.h"
#include "StdArray.h"
#include "Async.h"
#include "Timer.h"
#include "string.h"
#include "test.h"

namespace test
{
    void testArray_spesific(auto &arr)
    {
        std::cout << "display arr\n"
                  << arr << std::endl;
        //------------------------push
        int num = 13;
        std::cout << "int num = 13, arr.push(&num) returns " << arr.push(&num) << ", and arr is\n"
                  << arr << std::endl;
        //------------------------unshift
        std::cout << "arr.unshift(-6, 2147483648) returns " << arr.unshift(-6, 2147483648) << ", and arr is\n"
                  << arr << std::endl;

        //------------------------pop
        std::cout << "arr.pop() returns " << arr.pop() << ", and arr is\n"
                  << arr << std::endl;
        //------------------------shift
        std::cout << "arr.shift() returns " << arr.shift() << ", and arr is\n"
                  << arr << std::endl;
        //------------------------at
        std::cout << "arr.at(2) returns " << arr.at(2) << ", and arr.at(-1) returns " << arr.at(-1) << std::endl;
        //------------------------indexOf
        std::cout << "arr.indexOf(2147483648) returns " << arr.indexOf(2147483648) << std::endl;
        //------------------------lastIndexOf
        std::cout << "arr.lastIndexOf(2147483648) returns " << arr.lastIndexOf(2147483648) << std::endl;
        //------------------------incldues
        std::cout << "'8 is the element of arr' is " << arr.includes(8) << std::endl;

        //------------------------join
        std::cout << "arr.join() is\n\n"
                  << arr.join() << "\n\na.join(\" + \") is\n\n"
                  << arr.join(" + ") << "\n"
                  << std::endl;
        //------------------------forEach & operator=
        arr = {1, 5, 3, 7, 8, 9};
        std::cout << "arr = {1, 5, 3, 7, 8, 9} and the square of each value is\n\n";
        arr.forEach([](Any value, ...)
                    { std::cout << std::get<int>(value) * std::get<int>(value) << " "; });
        std::cout << "\n"
                  << std::endl;
        //------------------------some
        std::cout << "'arr includes even number' is " << arr.some([](Any value, ...)
                                                                  { return std::get<int>(value) % 2 == 0; })
                  << std::endl;
        //------------------------every
        std::cout << "'all the numbers in arr are positive' is " << arr.every([](Any value, ...)
                                                                              { return std::get<int>(value) > 0; })
                  << std::endl;
        //------------------------find
        std::cout << "the first number in arr which is larger than 5 is " << arr.find([](Any value, ...)
                                                                                      { return std::get<int>(value) > 5; })
                  << std::endl;
        //------------------------findLast
        std::cout << "the last number in arr which is smaller than 9 is " << arr.findLast([](Any value, ...)
                                                                                          { return std::get<int>(value) < 9; })
                  << std::endl;
        //------------------------findIndex
        std::cout << "the index of first number in arr which is larger than 5 is " << arr.findIndex([](Any value, ...)
                                                                                                    { return std::get<int>(value) > 5; })
                  << std::endl;
        //------------------------findLastIndex
        std::cout << "the index of last number in arr which is smaller than 9 is " << arr.findLastIndex([](Any value, ...)
                                                                                                        { return std::get<int>(value) < 9; })
                  << std::endl;
        //------------------------reduce
        std::cout << "the sum of numbers in arr(with initial value 100) is " << arr.reduce([](Any a, Any b, ...)
                                                                                           { return std::get<int>(a) + std::get<int>(b); },
                                                                                           100)
                  << std::endl;
        //------------------------reduceRight
        std::cout << "the product of numbers in arr meanwhile adding their index(from right side) is " << arr.reduceRight([](Any a, Any b, size_t crtIndex, ...)
                                                                                                                          { Any r = std::get<int>(a) * std::get<int>(b);
                                                                                                              return std::get<int>(r) += crtIndex; })
                  << std::endl;
        //------------------------filter
        std::cout << "the array that contains the numbers larger than 3 in arr\n"
                  << arr.filter([](Any value, ...)
                                { return std::get<int>(value) > 3; })
                  << std::endl;
        //------------------------map
        std::cout << "the array that equals each number in arr multiplied by 0.3\n"
                  << arr.map([](Any value, ...)
                             { return (double)(std::get<int>(value) * 0.3); })
                  << std::endl;
        //------------------------concat
        std::cout << "the array that euquals arr concated with {\"str\", 0.5, false}\n"
                  << arr.concat({"str", 0.5, false}) << std::endl;
        //------------------------slice
        std::cout << "the array that equals the slice of arr within index [1, 4)\n"
                  << arr.slice(1, 4) << std::endl;
        //------------------------splice
        std::cout << "slice arr from index 1, delete two elements and splice with {-1, -2, -3} returns\n"
                  << arr.splice(1, 2, {-1, -2, -3})
                  << "\nand arr is\n"
                  << arr << std::endl;
        //------------------------reverse
        std::cout << "reverse arr\n"
                  << arr.reverse() << std::endl;
        //------------------------sort
        std::cout << "sort arr by default order\n"
                  << arr.sort() << "\nand sort arr by descending order\n"
                  << arr.sort([](Any a, Any b, ...)
                              { return a > b ? -1 : 1; })
                  << std::endl;
        //------------------------copyWithin
        std::cout << "copy the numbers in arr within index [4, 6) to target index 0\n"
                  << arr.copyWithin(0, 4, 6) << std::endl;
        //------------------------fill
        std::cout << "fill arr with value 10 within index [0, 3)\n"
                  << arr.fill(10, 0, 3) << std::endl;
        //------------------------operator[]
        arr[9] = -5;
        std::cout << "arr[9] = -5, and now arr is\n"
                  << arr << std::endl;
    }

    void testArray()
    {
        {
            Timer timer("StdArray Test");
            StdArray<Any> arr = {3.5, true, 2147483648, nullptr, "this is string"};
            test::testArray_spesific(arr);
        }
        std::cout << "----------------------------------------------------------" << std::endl;
        {
            Timer timer("RawArray Test");
            RawArray<Any> arr = {3.5, true, 2147483648, nullptr, "this is string"};
            test::testArray_spesific(arr);
        }
    }

    void testAsync()
    {
        using namespace std::chrono_literals;
        AsyncFunction<std::string, Any> f(
            [&](Any data)
            {
                std::this_thread::sleep_for(1000ms);
                auto type = std::visit([](auto &v)
                                       { return typeid(v).name(); },
                                       data);
                return std::string("typeof data is ").append(type);
            });
        f(nullptr).then([](std::string rst)
                        { std::cout << rst << std::endl; });
    }

    void testString()
    {
        string s = "我？我是Tom";
        std::cout << "s = " << s << ", s.length = " << s.length << std::endl;
        std::cout << "s.charAt(0) = " << s.charAt(0) << std::endl;
        std::cout << "s.charCodeAt(3) = " << s.charAt(3) << ": " << s.charCodeAt(3) << std::endl;
        std::cout << "s.concat({\"，\", \"你叫什么名字？\"}) = "
                  << s.concat({"，", "你叫什么名字？"}) << std::endl;
        std::cout << "s.startsWith(\"我\") = "
                  << s.startsWith("我") << std::endl;
        std::cout << "s.endsWith(\"m\", 3) = "
                  << s.endsWith("m", 3) << ", but "
                  << "s.endsWith(\"m\", 7) = "
                  << s.endsWith("m", 7)
                  << std::endl;
        std::cout << "s.replace(\"我\", \"他\") = "
                  << s.replace("我", "他") << std::endl;
        std::cout << "s.slice(1, 7) = "
                  << s.slice(1, 7) << std::endl;
        std::cout << "s.substring(-100, 2) = "
                  << s.substring(-100, 2) << ", and "
                  << "s.substring(2, -100) = "
                  << s.substring(2, -100)
                  << std::endl;
        std::cout << "s.substr(1, 1) = "
                  << s.substr(1, 1) << std::endl;
    }
} // namespace test
