#include "CES/String.h"

namespace CES
{
    //////////////////////////////////Constructor
    String::String(char c8) : m_str(1, c8), length{1} {}
    String::String(char16_t c16) : m_str{std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t>{}.to_bytes(c16)}, length{1} {}
    //////////////////////////////////ES Method
    String String::charAt(ptrdiff_t index) const
    {
        if (index < 0 || index >= length)
            return String{};
        std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cvt;
        char16_t at = cvt.from_bytes(m_str)[index];
        return String{cvt.to_bytes(at)};
    }
    uint16_t String::charCodeAt(ptrdiff_t index) const { return std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t>{}.from_bytes(m_str)[index]; }
    String String::concat(std::initializer_list<String> strings) const
    {
        std::string concatStr(m_str);
        for (const String &str : strings)
            concatStr.append(str.m_str);
        return concatStr;
    }
    bool String::startsWith(const String &s, std::optional<ptrdiff_t> _position) const
    {
        ptrdiff_t position = (_position.has_value() ? *_position : 0);
        if (position > length)
            return false;
        position < 0 && (position = 0);
        std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cvt;
        std::u16string u16str = cvt.from_bytes(m_str);
        return std::u16string_view{u16str.data() + position}.starts_with(cvt.from_bytes(s.m_str));
    }
    bool String::endsWith(const String &s, std::optional<ptrdiff_t> _position) const
    {
        ptrdiff_t position = (_position.has_value() ? *_position : length);
        if (position <= 0)
            return false;
        position > length && (position = length);
        std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cvt;
        std::u16string u16str = cvt.from_bytes(m_str);
        return std::u16string_view{u16str.data(), (size_t)position}.ends_with(cvt.from_bytes(s.m_str));
    }
    String String::replace(const String &search, const String &rep) const
    {
        std::string tmp = m_str;
        size_t findIndex = 0;
        while ((findIndex = tmp.find(search.m_str)) != std::string::npos)
        {
            tmp.replace(tmp.begin() + findIndex, tmp.begin() + findIndex + rep.m_str.size(), rep.m_str);
        }
        return String{tmp};
    }
    String String::slice(ptrdiff_t start, std::optional<ptrdiff_t> _end) const
    {
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length) || end > length && (end = length);
        if (end <= start)
            return String{};
        std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cvt;
        std::u16string u16str = cvt.from_bytes(m_str);
        return String{cvt.to_bytes(u16str.c_str() + start, u16str.c_str() + end)};
    }
    String String::substring(ptrdiff_t start, std::optional<ptrdiff_t> _end) const
    {
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        start < 0 && (start = 0);
        end < 0 && (end = 0);
        if (end == start)
            return String{};
        if (end < start)
            std::swap(start, end);
        return slice(start, end);
    }
    String String::substr(ptrdiff_t from, std::optional<ptrdiff_t> _len) const
    {
        ptrdiff_t len = (_len.has_value() ? *_len : length - from);
        len > length - from && (len = length - from);
        return slice(from, from + len);
    }
    //////////////////////////////////Operator
    std::ostream &operator<<(std::ostream &os, const String &str)
    {
        return (os << str.m_str);
    }
} // CES