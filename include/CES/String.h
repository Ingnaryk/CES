#ifndef _STRING_H_
#define _STRING_H_

#include <codecvt>
#include <locale>
#include <string_view>
#include <optional>
#include <iomanip>
#include <cstdint>
#include <utility>
#include <limits>

namespace CES
{
    class String
    {
        using index_type = ptrdiff_t;
        using wm_converter = std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t>;

    private:
        //////////////////////////////////Wrapped data
        std::string m_str;

    public:
        //////////////////////////////////ES Property
        const index_type length{0};
        //////////////////////////////////Constructor
        String(char c8);
        String(char16_t c16);
        template <typename... Args>
            requires std::constructible_from<decltype(m_str), Args...>
        String(Args &&...args) : m_str{std::forward<Args>(args)...}
        {
            auto size = wm_converter{}.from_bytes(m_str).size();
            if (size > std::numeric_limits<index_type>::max()) [[unlikely]]
                throw std::overflow_error("String length is too large!");
            const_cast<index_type &>(length) = static_cast<index_type>(size);
        }
        String(String &&other) noexcept : m_str{std::move(other.m_str)}, length{std::exchange(const_cast<index_type &>(other.length), 0)} {}
        //////////////////////////////////ES Method
        String charAt(index_type index) const;
        uint16_t charCodeAt(index_type index) const;
        String concat(std::initializer_list<String> strings) const;
        bool startsWith(const String &s, std::optional<index_type> _position = std::nullopt) const noexcept;
        bool endsWith(const String &s, std::optional<index_type> _position = std::nullopt) const noexcept;
        String replace(const String &search, const String &rep) const;
        String slice(index_type start = 0, std::optional<index_type> _end = std::nullopt) const;
        String substring(index_type start, std::optional<index_type> _end = std::nullopt) const;
        String substr(index_type from, std::optional<index_type> _len = std::nullopt) const;
        //////////////////////////////////Operator, no operator[]!
        String &operator=(String &&) noexcept;
        friend std::ostream &operator<<(std::ostream &os, const String &str);
    };
} // CES

#endif // _STRING_H_