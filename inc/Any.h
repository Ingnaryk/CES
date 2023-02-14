#ifndef _ANY_H_
#define _ANY_H_

#include <variant>
#include <initializer_list>
#include <functional>
#include <optional>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <utility>

#define undefined nullptr
using any = std::variant<void *, int, long long, bool, double, std::string>;

// visitor helper
template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
#if __cplusplus < 202002L
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
#endif

constexpr const char *boolean(bool arg);
std::ostream &operator<<(std::ostream &os, const any &value);

#endif // _ANY_H_