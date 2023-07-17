#ifndef _ANY_H_
#define _ANY_H_

#include <variant>
#include <functional>
#include <optional>
#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <utility>

#include "type.h"

#define undefined nullptr

namespace CES
{
    using any = std::variant<void *, int, long long, bool, double, std::string>;

    std::string to_string(const any &value);
}

std::ostream &operator<<(std::ostream &os, const CES::any &value);

template <typename First, typename Second>
constexpr std::ostream &operator<<(std::ostream &os, const std::pair<First, Second> &pair)
{
    return (os << "(" << pair.first << ", " << pair.second << ")");
}

#endif // _ANY_H_
