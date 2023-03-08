#include "StdArray.h"
#include "RawArray.h"
#include "RawList.h"
#include "async.h"
#include "timer.h"
#include "string.h"
#include "test.h"

namespace test
{
    int testType::ctor = 0;
    int testType::cctor = 0;
    int testType::mctor = 0;
    int testType::cass = 0;
    int testType::mass = 0;
    int testType::dtor = 0;

    void demo_StdArray_RawArray_RawList()
    {
        static auto testArray_spesific = [](auto &arr)
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
            arr.forEach([](any value, ...)
                        { std::cout << std::get<int>(value) * std::get<int>(value) << " "; });
            std::cout << "\n"
                      << std::endl;
            //------------------------some
            std::cout << "'arr includes even number' is " << arr.some([](any value, ...)
                                                                      { return std::get<int>(value) % 2 == 0; })
                      << std::endl;
            //------------------------every
            std::cout << "'all the numbers in arr are positive' is " << arr.every([](any value, ...)
                                                                                  { return std::get<int>(value) > 0; })
                      << std::endl;
            //------------------------find
            std::cout << "the first number in arr which is larger than 5 is " << arr.find([](any value, ...)
                                                                                          { return std::get<int>(value) > 5; })
                      << std::endl;
            //------------------------findLast
            std::cout << "the last number in arr which is smaller than 9 is " << arr.findLast([](any value, ...)
                                                                                              { return std::get<int>(value) < 9; })
                      << std::endl;
            //------------------------findIndex
            std::cout << "the index of first number in arr which is larger than 5 is " << arr.findIndex([](any value, ...)
                                                                                                        { return std::get<int>(value) > 5; })
                      << std::endl;
            //------------------------findLastIndex
            std::cout << "the index of last number in arr which is smaller than 9 is " << arr.findLastIndex([](any value, ...)
                                                                                                            { return std::get<int>(value) < 9; })
                      << std::endl;
            //------------------------reduce
            std::cout << "the sum of numbers in arr(with initial value 100) is " << arr.reduce([](any a, any b, ...)
                                                                                               { return std::get<int>(a) + std::get<int>(b); },
                                                                                               100)
                      << std::endl;
            //------------------------reduceRight
            std::cout << "the product of numbers in arr meanwhile adding their index(from right side) is " << arr.reduceRight([](any a, any b, ptrdiff_t crtIndex, ...)
                                                                                                                              { any r = std::get<int>(a) * std::get<int>(b);
                                                                                                              return std::get<int>(r) += crtIndex; })
                      << std::endl;
            //------------------------filter
            std::cout << "the array that contains the numbers larger than 3 in arr\n"
                      << arr.filter([](any value, ...)
                                    { return std::get<int>(value) > 3; })
                      << std::endl;
            //------------------------map
            std::cout << "the array that equals each number in arr multiplied by 0.3\n"
                      << arr.template map<double>([](any value, ...)
                                                  { return (double)(std::get<int>(value) * 0.3); })
                      << std::endl;
            //------------------------concat
            std::cout << "arr.concat({\"str\", 0.5, false}) returns\n"
                      << arr.concat({"str", 0.5, false}) << std::endl;
            //------------------------slice
            std::cout << "the array that equals the slice of arr within index [1, 4)\n"
                      << arr.slice(1, 4) << std::endl;
            //------------------------splice
            std::cout << "slice arr from index 1, delete 2 elements and splice with {-1, -2, -3} returns\n"
                      << arr.splice(1, 2, {-1, -2, -3})
                      << "\nand arr is\n"
                      << arr << std::endl;
            //------------------------reverse
            std::cout << "reverse arr\n"
                      << arr.reverse() << std::endl;
            //------------------------sort
            std::cout << "sort arr by default order\n"
                      << arr.sort() << "\nand sort arr by descending order\n"
                      << arr.sort([](any a, any b, ...)
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
        };
        timer::outputBuffer = 3;
        {
            timer t("StdArray<any> demo");
            StdArray<any> arr = {3.5, true, 2147483648, nullptr, "this is string"};
            testArray_spesific(arr);
        }
        std::cout << "\n\n";
        {
            timer t("RawArray<any> demo");
            RawArray<any> arr = {3.5, true, 2147483648, nullptr, "this is string"};
            testArray_spesific(arr);
        }
        std::cout << "\n\n";
        {
            timer t("RawList<any> demo");
            RawList<any> arr = {3.5, true, 2147483648, nullptr, "this is string"};
            testArray_spesific(arr);
        }
    }

    void test_RawList_stdlist()
    {
        timer::outputBuffer = 2;
        {
            timer t("RawList<int> adding 20k elements back and front continuously");
            RawList<int> a{1, 3, 5, 7, 9};
            for (int i = 0; i < 10000; i++)
            {
                a.push(a[i % a.length]);
                a.unshift(std::get<int>(a.at(-1 - i % a.length)));
            }
        }
        {
            timer t("std::list<int> adding 20k elements back and front continuously");
            std::list<int> a{1, 3, 5, 7, 9};
            for (int i = 0; i < 10000; i++)
            {
                std::list<int>::iterator itor = a.begin();
                std::advance(itor, i % a.size());
                a.emplace_back(*itor);
                itor = a.end();
                std::advance(itor, -1 - i % a.size());
                a.emplace_front(*itor);
            }
        }
    }

    void test_RawArray_stdvector()
    {

        timer::outputBuffer = 6;
        {
            timer t("RawArray<int> adding 30k elements back and front continuously");
            RawArray<int> a{1, 3, 5, 7, 9};
            for (int i = 0; i < 10000; i++)
            {
                a.push(a[i % a.length]);
                a.unshift(i, (int)a.length);
            }
        }
        {
            timer t("std::vector<int> adding 30k elements back and front continuously");
            std::vector<int> a{1, 3, 5, 7, 9};
            for (int i = 0; i < 10000; i++)
            {
                a.emplace_back(a[i % a.size()]);
                a.insert(a.begin(), {i, (int)a.size()});
            }
        }
        {
            timer t("RawArray<testType> adding 3k elements back and front continuously");
            RawArray<testType> a{1, 3, 5, 7, 9};
            for (int i = 0; i < 1000; i++)
            {
                a.push(a[i % a.length]);
                a.unshift(i, (int)a.length);
            }
            std::cout << testType::ctor << '\t' << testType::cctor << '\t' << testType::mctor << '\t' << testType::cass << '\t' << testType::mass << '\t' << testType::dtor << "\nTotal: " << testType::ctor + testType::cctor + testType::mctor + testType::cass + testType::mass + testType::dtor << '\n';
        }
        testType::ctor = testType::cctor = testType::mctor = testType::cass = testType::mass = testType::dtor = 0;
        {
            timer t("std::vector<testType> adding 3k elements back and front continuously");
            std::vector<testType> a{1, 3, 5, 7, 9};
            for (int i = 0; i < 1000; i++)
            {
                a.emplace_back(a[i % a.size()]);
                a.insert(a.begin(), {i, (int)a.size()});
            }
            std::cout << testType::ctor << '\t' << testType::cctor << '\t' << testType::mctor << '\t' << testType::cass << '\t' << testType::mass << '\t' << testType::dtor << "\nTotal: " << testType::ctor + testType::cctor + testType::mctor + testType::cass + testType::mass + testType::dtor << '\n';
        }
        {
            timer t("RawArray<std::string> adding 3k elements back and front continuously");
            RawArray<std::string> a{"1", "3", "5", "7", "9"};
            for (int i = 0; i < 1000; i++)
            {
                a.push(a[i % a.length]);
                a.unshift(std::to_string(i), std::to_string(a.length));
            }
        }
        {
            timer t("std::vector<std::string> adding 3k elements back and front continuously");
            std::vector<std::string> a{"1", "3", "5", "7", "9"};
            for (int i = 0; i < 1000; i++)
            {
                a.emplace_back(a[i % a.size()]);
                a.insert(a.begin(), {std::to_string(i), std::to_string(a.size())});
            }
        }
    }

    void testAsync()
    {
        AsyncFunction<std::string> f(
            []()
            {
                std::this_thread::sleep_for(1000ms);
                std::string s;
                std::cin >> s;
                return s;
            });
        f().then([](std::string rst)
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
