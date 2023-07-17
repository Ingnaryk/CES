#ifndef _TYPE_H_
#define _TYPE_H_

#include <string_view>

template <typename T>
constexpr std::string_view type()
{
    constexpr std::string_view pretty_function = __PRETTY_FUNCTION__;
    constexpr std::size_t begin = pretty_function.find_first_of('=') + 2;
    constexpr std::size_t end = std::min(pretty_function.find_first_of('<'), pretty_function.find_last_of(']'));
    return pretty_function.substr(begin, end - begin);
}

// visitor for std::variant
template <typename... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
#if __cplusplus < 202002L
// explicit deduction guide (not needed as of C++20)
template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
#endif

#endif // _TYPE_H_