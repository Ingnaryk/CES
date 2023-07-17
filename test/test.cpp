#include <cassert>

#include "CES/type.h"
#include "test/test.h"

namespace test
{
    void demo_Array()
    {
        CES::Array<CES::any> arr{3.5, true, 2147483648, nullptr, "this is string"};
        std::cout << "display arr (" << type<decltype(arr)>() << ")\n"
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
        std::cout << "arr.includes(8) returns " << arr.includes(8) << std::endl;

        //------------------------join
        std::cout << "arr.join() is\n\n"
                  << arr.join() << "\n\na.join(\" + \") is\n\n"
                  << arr.join(" + ") << "\n"
                  << std::endl;
        //------------------------forEach & operator=
        arr = {1, 5, 3, 7, 8, 9};
        std::cout << "arr = {1, 5, 3, 7, 8, 9} and the square of each value is\n\n";
        arr.forEach([](CES::any value)
                    { std::cout << std::get<int>(value) * std::get<int>(value) << " "; });
        std::cout << "\n"
                  << std::endl;
        //------------------------some
        std::cout << "'even number exists in arr' is " << arr.some([](CES::any value)
                                                                   { return std::get<int>(value) % 2 == 0; })
                  << std::endl;
        //------------------------every
        std::cout << "'all the numbers in arr are positive' is " << arr.every([](CES::any value)
                                                                              { return std::get<int>(value) > 0; })
                  << std::endl;
        //------------------------find
        std::cout << "the first number in arr which is larger than 5 is " << arr.find([](CES::any value)
                                                                                      { return std::get<int>(value) > 5; })
                  << std::endl;
        //------------------------findLast
        std::cout << "the last number in arr which is smaller than 9 is " << arr.findLast([](CES::any value)
                                                                                          { return std::get<int>(value) < 9; })
                  << std::endl;
        //------------------------findIndex
        std::cout << "the index of first number in arr which is larger than 5 is " << arr.findIndex([](CES::any value)
                                                                                                    { return std::get<int>(value) > 5; })
                  << std::endl;
        //------------------------findLastIndex
        std::cout << "the index of last number in arr which is smaller than 9 is " << arr.findLastIndex([](CES::any value)
                                                                                                        { return std::get<int>(value) < 9; })
                  << std::endl;
        //------------------------reduce
        std::cout << "the sum of numbers in arr(with initial value 100) is " << arr.reduce([](CES::any a, CES::any b)
                                                                                           { return std::get<int>(a) + std::get<int>(b); },
                                                                                           100)
                  << std::endl;
        //------------------------reduceRight
        std::cout << "the product of numbers in arr meanwhile adding their index(from right side) is " << arr.reduceRight([](CES::any a, CES::any b, ptrdiff_t crtIndex)
                                                                                                                          { CES::any r = std::get<int>(a) * std::get<int>(b);
                                                                                                              return std::get<int>(r) += crtIndex; })
                  << std::endl;
        //------------------------filter
        std::cout << "the array that contains the numbers larger than 3 in arr\n"
                  << arr.filter([](CES::any value)
                                { return std::get<int>(value) > 3; })
                  << std::endl;
        //------------------------map
        std::cout << "the array that equals each number in arr multiplied by 0.3\n"
                  << arr.template map<double>([](CES::any value)
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
                  << arr.sort([](CES::any a, CES::any b)
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

    void demo_Map()
    {
        CES::Map<int, float> map;
        map.set(2, 6.6f), map.set(-5, -985.211f);
        std::cout << "display map (" << type<decltype(map)>() << ")\n"
                  << map << std::endl;
        std::cout << "map.get(2) returns " << map.get(2) << std::endl;
        std::cout << "map.get(789) returns " << map.get(789) << std::endl;
        std::cout << "map.has(-5) returns " << map.has(-5) << std::endl;
        std::cout << "map.Delete(1) returns " << map.Delete(1) << std::endl;
        std::cout << "map.Delete(-5) returns " << map.Delete(-5) << ", now map is\n"
                  << map << std::endl;
        map.clear();
        std::cout << "after map.clear(), display map\n"
                  << map << std::endl;
        map.set(-5, 4.3f), map.set(72, -75.5f);
        std::cout << "display map\n"
                  << map << std::endl;
        std::cout << "each element of map plus its key\n"
                  << std::endl;
        map.forEach([](float value, int key)
                    { std::cout << value + key << " "; });
        std::cout << "\n\nm.keys() returns\n"
                  << map.keys() << std::endl;
        std::cout << "map.values() returns\n"
                  << map.values() << std::endl;
        std::cout << "map.entries() returns\n"
                  << map.entries() << std::endl;
    }

    void demo_String()
    {
        CES::String str = "我？我是Tom";
        std::cout << "str = " << str << ", str.length = " << str.length << std::endl;
        std::cout << "str.charAt(0) = " << str.charAt(0) << std::endl;
        std::cout << "str.charCodeAt(3) = " << str.charAt(3) << ": " << str.charCodeAt(3) << std::endl;
        std::cout << "str.concat({\"，\", \"你叫什么名字？\"}) = "
                  << str.concat({"，", "你叫什么名字？"}) << std::endl;
        std::cout << "str.startsWith(\"我\", 2) = "
                  << str.startsWith("我", 2) << ", but "
                  << "str.startsWith(\"我\", 3) = "
                  << str.startsWith("我", 3) << std::endl;
        std::cout << "str.endsWith(\"m\", 6) = "
                  << str.endsWith("m", 6) << ", but "
                  << "str.endsWith(\"m\", 7) = "
                  << str.endsWith("m", 7)
                  << std::endl;
        std::cout << "str.replace(\"我\", \"他\") = "
                  << str.replace("我", "他") << std::endl;
        std::cout << "str.slice(1, 7) = "
                  << str.slice(1, 7) << std::endl;
        std::cout << "str.substring(-100, 2) = "
                  << str.substring(-100, 2) << ", and "
                  << "str.substring(2, -100) = "
                  << str.substring(2, -100)
                  << std::endl;
        std::cout << "str.substr(1, 1) = "
                  << str.substr(1, 1) << std::endl;
    }

    void demo_async()
    {
        AsyncFunction<std::string> waitInput{
            []()
            {
                std::string s;
                std::cin >> s;
                return s;
            }};
        waitInput().then([&](std::string_view rst)
                         { std::cout << "async function 'waitInput' returns " << rst << std::endl; });
        // while using functions like 'setTimeout', 'setInterval', etc. be ready to use shared_ptr
        // closures in c++ won't extend the life of the reference of variables on the stack
        auto value = std::make_shared<std::optional<int>>(3);
        std::cout << "[main thread]: the value is " << value->value() << std::endl;
        setTimeout([](auto value)
                   { std::cout << "[timer thread]: the value is " << value->value() << std::endl; },
                   1000ms, value);
        value->emplace(4);
        std::cout << "[main thread]: change the value into " << value->value() << " and wait for the delay 1000ms output, pass pointer"
                  << std::endl;
        std::this_thread::sleep_for(1500ms);
        setTimeout([value]()
                   { std::cout << "[timer thread]: the value is " << value->value() << std::endl; },
                   1000ms);
        value->emplace(5);
        std::cout << "[main thread]: change the value into " << value->value() << " and wait for the delay 1000ms output, use closure"
                  << std::endl;
        std::this_thread::sleep_for(1500ms);
        setTimeout([value]()
                   { value->emplace(8);
                        std::cout << "[timer thread]: modify the value to " << value->value() << std::endl; },
                   1000ms);
        std::cout << "[main thread]: change the value in the timer" << std::endl;
        std::this_thread::sleep_for(1500ms);
        std::cout << "[main thread]: now the value is " << value->value() << std::endl;
        std::this_thread::sleep_for(500ms);
        size_t timerId = setTimeout([value]()
                                    { value->reset();
                                      std::cout << "[timer thread]: I have cleared the value" << std::endl; },
                                    1000ms);
        std::cout << "[main thread]: delay 1000ms to clear the value (timerId = " << timerId << ")" << std::endl;
        std::this_thread::sleep_for(500ms);
        clearTimeout(timerId);
        std::cout << "[main thread]: use clearTimeout to block timer's modification to the value (timerId = " << timerId << ")" << std::endl;
        std::this_thread::sleep_for(1000ms);
        assert(value->has_value());
        std::cout << "[main thread]: now the value is still " << value->value() << std::endl;
        setInterval([]()
                    { static int count = 0; std::cout << "[timer thread]: counter " << ++count << std::endl; },
                    1000ms);
    }
} // namespace test
