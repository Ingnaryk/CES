#include "Any.h"

constexpr const char *boolean(bool &arg)
{
    return arg ? "true" : "false";
}
std::ostream &operator<<(std::ostream &os, const Any &value)
{
    visit(
        overloaded{[&os](auto arg)
                   { os << arg; },
                   [&os](bool arg)
                   { os << boolean(arg); },
                   [&os](double arg)
                   { os << std::fixed << arg; },
                   [&os](const std::string &arg)
                   { os << (typeid(os) == typeid(std::ostream) ? ("\"" + arg + "\"") : arg); },
                   [&os](void *arg)
                   { char s[25];
                     sprintf(s, (arg != nullptr ? "address(%p)" : "undefined"), arg);
                     os << s; },
                   [&os](std::nullptr_t arg)
                   { os << "undefined"; }},
        value);
    return os;
}