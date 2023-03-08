#include "string.h"

//////////////////////////////////Constructor
string::string(char c) : m_str(1, c), length(1) {}
string::string(const char *str) : m_str(str), length(strlen(str)) {}
string::string(const std::string &str) : m_str(str), length(strlen(str.c_str())) {}
//////////////////////////////////ES Method
string string::charAt(size_t index) const
{
    if (index < 0 || index >= length)
        return string{""};
    wchar_t at = wcv.from_bytes(m_str)[index];
    return string{wcv.to_bytes(at)};
}
uint16_t string::charCodeAt(size_t index) const
{
    return wcv.from_bytes(m_str)[index];
}
string string::concat(const std::initializer_list<string> &strings) const
{
    std::string concatStr(m_str);
    for (const string &str : strings)
        concatStr.append(str.m_str);
    return concatStr;
}
bool string::startsWith(const string &s, std::optional<ptrdiff_t> _position) const
{
    ptrdiff_t position = (_position.has_value() ? *_position : 0);
    if (position > length)
        return false;
    position = (position < 0 ? 0 : position);
    std::wstring tmp = wcv.from_bytes(m_str);
    std::wstring_view wstrv{tmp.begin() + position, tmp.end()};
    return wstrv.starts_with(wcv.from_bytes(s.m_str).c_str());
}
bool string::endsWith(const string &s, std::optional<ptrdiff_t> _position) const
{
    ptrdiff_t position = (_position.has_value() ? *_position : length);
    if (position <= 0)
        return false;
    position = (position > length ? length : position);
    std::wstring tmp = wcv.from_bytes(m_str);
    std::wstring_view wstrv{tmp.begin(), tmp.begin() + position};
    return wstrv.ends_with(wcv.from_bytes(s.m_str).c_str());
}
string string::replace(const string &search, const string &rep) const
{
    std::string tmp = m_str;
    size_t findIndex = 0;
    while ((findIndex = tmp.find(search.m_str)) != std::string::npos)
    {
        tmp.replace(tmp.begin() + findIndex, tmp.begin() + findIndex + rep.m_str.size(), rep.m_str);
    }
    return string{tmp};
}
string string::slice(ptrdiff_t start, std::optional<ptrdiff_t> _end) const
{
    ptrdiff_t end = (_end.has_value() ? *_end : length);
    (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
    (end < 0 && end > -length) && (end += length) || end > length && (end = length);
    if (start >= length || end <= start)
        return string{""};
    std::wstring tmp = wcv.from_bytes(m_str);
    return string{wcv.to_bytes(tmp.c_str() + start, tmp.c_str() + end)};
}
string string::substring(ptrdiff_t start, std::optional<ptrdiff_t> _end) const
{
    ptrdiff_t end = (_end.has_value() ? *_end : length);
    start < 0 && (start = 0);
    end < 0 && (end = 0);
    if (end == start)
        return string{""};
    if (end < start)
        std::swap(start, end);
    return slice(start, end);
}
string string::substr(ptrdiff_t from, std::optional<ptrdiff_t> _len) const
{
    ptrdiff_t len = (_len.has_value() ? *_len : length - from);
    len > length - from && (len = length - from);
    return slice(from, from + len);
}
//////////////////////////////////Operator
std::ostream &operator<<(std::ostream &os, const string &str)
{
    return (os << str.m_str);
}