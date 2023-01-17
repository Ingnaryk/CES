#include "../inc/RawArray.h"
#include "../inc/StdArray.h"
#include "../inc/Async.h"
#include "../inc/string.h"
#include "../inc/Timer.h"
#include "../inc/Test.h"

namespace Test
{
    void testArray()
    {
        {
            Timer timer("StdArray Test");
            StdArray<Any> arr = {3.5, true, 2147483648, nullptr, "this is string"};
            Test::testArray_spesific(arr);
        }
        std::cout << "----------------------------------------------------------" << std::endl;
        {
            Timer timer("RawArray Test");
            RawArray<Any> arr = {3.5, true, 2147483648, nullptr, "this is string"};
            Test::testArray_spesific(arr);
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
}