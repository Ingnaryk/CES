#include "CES/String.h"

namespace CES
{
    //////////////////////////////////Constructor
    String::String(char c8) : m_str(1, c8), length{1} {}
    String::String(char16_t c16) : m_str{wm_converter{}.to_bytes(c16)}, length{1} {}
    //////////////////////////////////ES Method
    String String::charAt(index_type index) const
    {
        if (index < 0 || index >= length)
            return String{};
        wm_converter cvt;
        char16_t at = cvt.from_bytes(m_str)[index];
        return String{cvt.to_bytes(at)};
    }
    uint16_t String::charCodeAt(index_type index) const { return wm_converter{}.from_bytes(m_str)[index]; }
    String String::concat(std::initializer_list<String> strings) const
    {
        std::string concatStr(m_str);
        for (const String &str : strings)
            concatStr.append(str.m_str);
        if (concatStr.size() > std::numeric_limits<index_type>::max()) [[unlikely]]
            throw std::overflow_error("String length is too large!");
        return concatStr;
    }
    bool String::startsWith(const String &s, std::optional<index_type> _position) const noexcept
    {
        index_type position = (_position.has_value() ? *_position : 0);
        if (position > length)
            return false;
        position < 0 && (position = 0);
        wm_converter cvt;
        std::u16string u16str = cvt.from_bytes(m_str);
        return std::u16string_view{u16str.data() + position}.starts_with(cvt.from_bytes(s.m_str));
    }
    bool String::endsWith(const String &s, std::optional<index_type> _position) const noexcept
    {
        index_type position = (_position.has_value() ? *_position : length);
        if (position <= 0)
            return false;
        position > length && (position = length);
        wm_converter cvt;
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
    String String::slice(index_type start, std::optional<index_type> _end) const
    {
        index_type end = (_end.has_value() ? *_end : length);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length) || end > length && (end = length);
        if (end <= start)
            return String{};
        wm_converter cvt;
        std::u16string u16str = cvt.from_bytes(m_str);
        return String{cvt.to_bytes(u16str.c_str() + start, u16str.c_str() + end)};
    }
    String String::substring(index_type start, std::optional<index_type> _end) const
    {
        index_type end = (_end.has_value() ? *_end : length);
        start < 0 && (start = 0);
        end < 0 && (end = 0);
        if (end == start)
            return String{};
        if (end < start)
            std::swap(start, end);
        return slice(start, end);
    }
    String String::substr(index_type from, std::optional<index_type> _len) const
    {
        index_type len = (_len.has_value() ? *_len : length - from);
        len > length - from && (len = length - from);
        return slice(from, from + len);
    }
    //////////////////////////////////Operator
    String &String::operator=(String &&other) noexcept
    {
        if (this == &other) [[unlikely]]
            return *this;
        m_str = std::move(other.m_str);
        const_cast<index_type &>(length) = std::exchange(const_cast<index_type &>(other.length), 0);
        return *this;
    }
    std::ostream &operator<<(std::ostream &os, const String &str)
    {
        return (os << str.m_str);
    }
} // CES