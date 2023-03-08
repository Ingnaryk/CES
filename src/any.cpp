#include "any.h"

constexpr const char *boolean(bool arg)
{
    return arg ? "true" : "false";
}
std::ostream &operator<<(std::ostream &os, const any &value)
{
    std::visit(
        overloaded{[&os](auto arg)
                   { os << arg; },
                   [&os](bool arg)
                   { os << boolean(arg); },
                   [&os](double arg)
                   { os << std::fixed << arg; },
                   [&os](const std::string &arg)
                   { os << (typeid(os) == typeid(std::ostream) ? ("\"" + arg + "\"") : arg); },
                   [&os](void *arg)
                   { if (arg != nullptr)
                        os << "address(" << arg << ")";
                     else
                        os << "undefined"; },
                   [&os](std::nullptr_t arg)
                   { os << "undefined"; }},
        value);
    return os;
}
