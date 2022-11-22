#include "../inc/Any.h"
using namespace std;

constexpr const char *boolean(bool &arg)
{
    return arg ? "true" : "false";
}
ostream &operator<<(ostream &os, const Any &value)
{
    visit(
        overloaded{[&os](auto arg)
                   { os << arg; },
                   [&os](bool arg)
                   { os << boolean(arg); },
                   [&os](double arg)
                   { os << fixed << arg; },
                   [&os](const string &arg)
                   { os << (typeid(os) == typeid(ostream) ? ("\"" + arg + "\"") : arg); },
                   [&os](void *arg)
                   { char s[25];
                     sprintf(s, (arg != nullptr ? "address(%p)" : "undefined"), arg);
                     os << s; },
                   [&os](nullptr_t arg)
                   { os << "undefined"; }},
        value);
    return os;
}