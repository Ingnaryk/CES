#ifndef _STRING_H_
#define _STRING_H_

#include "../inc/Any.h"

class string
{
private:
    //////////////////////////////////Wrapped data
    std::string m_str;

public:
    //////////////////////////////////ES Property
    const size_t length{0};
    //////////////////////////////////Constructor
    string() noexcept = default;
    string(char c);
    string(const char *str);
    string(const std::string &str);
    //////////////////////////////////ES Method
    string charAt(size_t index) const;
    uint16_t charCodeAt(size_t index) const;
    string concat(const std::initializer_list<string> &strings) const;
    bool startsWith(const string &s, std::optional<ptrdiff_t> _position = std::nullopt) const;
    bool endsWith(const string &s, std::optional<ptrdiff_t> _position = std::nullopt) const;
    string replace(const string &search, const string &rep) const;
    string slice(ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt) const;
    string substring(ptrdiff_t start, std::optional<ptrdiff_t> _end = std::nullopt) const;
    string substr(ptrdiff_t from, std::optional<ptrdiff_t> _len = std::nullopt) const;
    // slice, substring, substr
    //////////////////////////////////Operator
    friend std::ostream &operator<<(std::ostream &os, const string &str);
};

#endif // _STRING_H_