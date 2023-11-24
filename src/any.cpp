#include "CES/any.h"

namespace CES
{
    std::string to_string(const any &value)
    {
        using namespace std::string_literals;
        return std::visit(
            overloaded{[](auto number)
                       { return std::to_string(number); },
                       [](bool b)
                       { return b ? "true"s : "false"s; },
                       [](const std::string &str)
                       { return str; },
                       [](void *ptr)
                       {
                           if (ptr == nullptr)
                               return "undefined"s;
                           char buffer[32];
                           sprintf(buffer, "%p", ptr);
                           return std::string{buffer};
                       }},
            value);
    }
} // CES

std::ostream &operator<<(std::ostream &os, const CES::any &value)
{
    std::visit(
        overloaded{
            [&os](auto arg)
            { os << arg; },
            [&os](bool arg)
            { os << (arg ? "true" : "false"); },
            [&os](double arg)
            { os << std::fixed << arg; },
            [&os](const std::string &arg)
            { os << std::quoted(arg); },
            [&os](void *arg)
            {
                if (arg != nullptr)
                    os << "address(" << arg << ")";
                else
                    os << "undefined";
            }},
        value);
    return os;
}