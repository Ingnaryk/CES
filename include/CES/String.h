#ifndef _STRING_H_
#define _STRING_H_

#include <codecvt>
#include <locale>
#include <string_view>
#include <optional>
#include <iomanip>
#include <cstdint>

namespace CES
{
    class String
    {
    private:
        //////////////////////////////////Wrapped data
        std::string m_str;

    public:
        //////////////////////////////////ES Property
        const ptrdiff_t length{0};
        //////////////////////////////////Constructor
        String(char c8);
        String(char16_t c16);
        template <typename... Args>
        String(Args &&...args) : m_str{std::forward<Args>(args)...}, length(std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t>{}.from_bytes(m_str).size()) {}
        //////////////////////////////////ES Method
        String charAt(ptrdiff_t index) const;
        uint16_t charCodeAt(ptrdiff_t index) const;
        String concat(std::initializer_list<String> strings) const;
        bool startsWith(const String &s, std::optional<ptrdiff_t> _position = std::nullopt) const;
        bool endsWith(const String &s, std::optional<ptrdiff_t> _position = std::nullopt) const;
        String replace(const String &search, const String &rep) const;
        String slice(ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt) const;
        String substring(ptrdiff_t start, std::optional<ptrdiff_t> _end = std::nullopt) const;
        String substr(ptrdiff_t from, std::optional<ptrdiff_t> _len = std::nullopt) const;
        //////////////////////////////////Operator, no operator[]!
        friend std::ostream &operator<<(std::ostream &os, const String &str);
    };
} // CES

#endif // _STRING_H_