#pragma once

#include <vector>
#include <ranges>
#include <numeric>
#include <limits>

#include "CES/any.h"

namespace CES
{
    template <typename T>
    class Array
    {
        using index_type = ptrdiff_t;

    private:
        //////////////////////////////////Internal usage
        template <typename R, bool reverse = false, typename ESCallback>
        constexpr auto basicAdaptor(ESCallback &&func)
        {
            index_type startIndex = reverse ? length - 1 : 0LL;
            if constexpr (std::is_invocable_r_v<R, ESCallback, T>)
                return func;
            else if constexpr (std::is_invocable_r_v<R, ESCallback, T, index_type>)
                return [func = std::forward<ESCallback>(func), startIndex](const T &value) mutable -> R
                {
                    return func(value, reverse ? startIndex-- : startIndex++);
                };
            else if constexpr (std::is_invocable_r_v<R, ESCallback, T, index_type, Array<T> &>)
                return [func = std::forward<ESCallback>(func), startIndex, this](const T &value) mutable -> R
                {
                    return func(value, reverse ? startIndex-- : startIndex++, std::ref(*this));
                };
        }
        template <typename R, bool hasInitialValue, bool reverse = false, typename ESCallback>
        constexpr auto reduceAdaptor(ESCallback &&func)
        {
            index_type startIndex = reverse ? (hasInitialValue ? length - 1 : length - 2)
                                            : (hasInitialValue ? 0LL : 1LL);
            using Init = std::conditional_t<hasInitialValue, R, T>;
            if constexpr (std::is_invocable_r_v<Init, ESCallback, Init, T>)
                return func;
            else if constexpr (std::is_invocable_r_v<Init, ESCallback, Init, T, index_type>)
                return [func = std::forward<ESCallback>(func), startIndex](const Init &initial, const T &value) mutable -> Init
                {
                    return func(initial, value, reverse ? startIndex-- : startIndex++);
                };
            else if constexpr (std::is_invocable_r_v<Init, ESCallback, Init, T, index_type, Array<T> &>)
                return [func = std::forward<ESCallback>(func), startIndex, this](const Init &initial, const T &value) mutable -> Init
                {
                    return func(initial, value, reverse ? startIndex-- : startIndex++, std::ref(*this));
                };
        }
        //////////////////////////////////Wrapped data
        std::vector<T> m_data{};

    public:
        //////////////////////////////////ES Property
        const index_type length{0};
        //////////////////////////////////Constructor
        Array() noexcept = default;
        explicit Array(index_type size) : m_data(size), length{size} {}
        Array(std::initializer_list<T> elements) : m_data{elements}
        {
            if (elements.size() > std::numeric_limits<index_type>::max()) [[unlikely]]
                throw std::overflow_error("Array length is too large!");
            const_cast<index_type &>(length) = static_cast<index_type>(elements.size());
        }
        template <std::input_iterator InputIt>
        Array(InputIt begin, InputIt end) : m_data{begin, end}
        {
            auto size = std::ranges::distance(begin, end);
            if (size > std::numeric_limits<index_type>::max()) [[unlikely]]
                throw std::overflow_error("Array length is too large!");
            const_cast<index_type &>(length) = static_cast<index_type>(size);
        }
        Array(Array &&other) noexcept : m_data{std::move(other.m_data)}, length{std::exchange(const_cast<index_type &>(other.length), 0)} {}
        //////////////////////////////////ES Method
        template <typename... Args>
        constexpr index_type push(T first, Args... rests)
        {
            m_data.insert(m_data.end(), std::initializer_list<T>{first, rests...});
            const_cast<index_type &>(length) = m_data.size();
            return length;
        }
        template <typename... Args>
        constexpr index_type unshift(T first, Args... rests)
        {
            m_data.insert(m_data.begin(), std::initializer_list<T>{first, rests...});
            const_cast<index_type &>(length) = m_data.size();
            return length;
        }
        constexpr T pop()
        {
            T back = std::move(m_data.back());
            m_data.pop_back();
            const_cast<index_type &>(length) -= 1;
            return back;
        }
        constexpr T shift()
        {
            T front = std::move(m_data.front());
            m_data.erase(m_data.begin());
            const_cast<index_type &>(length) -= 1;
            return front;
        }
        constexpr const T &at(index_type index) const
        {
            index < 0 && (index = (index + length) % length); // c++ feature: [-length, -1] % length returns itself!
            return m_data.at(index);
        }
        constexpr index_type indexOf(const T &value, index_type fromIndex = 0) const noexcept
        {
            if (fromIndex >= length) [[unlikely]]
                return -1;
            (fromIndex < 0 && fromIndex > -length) && (fromIndex += length) || fromIndex < 0 && (fromIndex = 0);
            auto findRange = m_data | std::views::drop(fromIndex);
            index_type index = std::ranges::find(findRange, value) - findRange.begin();
            return index == findRange.size() ? -1 : fromIndex + index;
        }
        constexpr index_type lastIndexOf(const T &value, index_type fromIndex = -1) const noexcept
        {
            (fromIndex < 0 && fromIndex >= -length) && (fromIndex += length) || fromIndex >= length && (fromIndex = length - 1);
            if (fromIndex < 0) [[unlikely]]
                return -1;
            auto findRange = m_data | std::views::take(fromIndex + 1) | std::views::reverse;
            index_type index = std::ranges::find(findRange, value) - findRange.begin();
            return index == findRange.size() ? -1 : fromIndex - index;
        }
        constexpr bool includes(const T &value) const noexcept
        {
            return std::ranges::find(m_data, value) != m_data.end();
        }
        constexpr std::string join(const char *separator = ",", std::string (*formatter)(const T &) = to_string) const noexcept
        {
            std::string s;
            for (index_type i = 0; i < length; ++i)
                s.operator+=(formatter(m_data[i])).operator+=((i == length - 1 ? "" : separator));
            return s;
        }
        template <typename ESCallback>
        constexpr void forEach(ESCallback &&callbackFn)
        {
            std::for_each(m_data.begin(), m_data.end(), basicAdaptor<void>(std::forward<ESCallback>(callbackFn)));
        }
        template <typename ESCallback>
        constexpr bool some(ESCallback &&predicate)
        {
            return std::any_of(m_data.begin(), m_data.end(), basicAdaptor<bool>(std::forward<ESCallback>(predicate)));
        }
        template <typename ESCallback>
        constexpr bool every(ESCallback &&predicate)
        {
            return std::all_of(m_data.begin(), m_data.end(), basicAdaptor<bool>(std::forward<ESCallback>(predicate)));
        }
        template <typename ESCallback>
        constexpr any find(ESCallback &&predicate)
        {
            auto value = std::ranges::find_if(m_data, basicAdaptor<bool>(std::forward<ESCallback>(predicate)));
            return value == m_data.end() ? undefined : *value;
        }
        template <typename ESCallback>
        constexpr any findLast(ESCallback &&predicate)
        {
            auto value = std::ranges::find_if(m_data.rbegin(), m_data.rend(), basicAdaptor<bool>(std::forward<ESCallback>(predicate)));
            return value == m_data.rend() ? undefined : *value;
        }
        template <typename ESCallback>
        constexpr index_type findIndex(ESCallback &&predicate)
        {
            index_type index = std::ranges::find_if(m_data, basicAdaptor<bool>(std::forward<ESCallback>(predicate))) - m_data.begin();
            return index == length ? -1 : index;
        }
        template <typename ESCallback>
        constexpr index_type findLastIndex(ESCallback &&predicate)
        {
            return length - 1 -
                   (std::ranges::find_if(
                        m_data.rbegin(),
                        m_data.rend(),
                        basicAdaptor<bool, true>(std::forward<ESCallback>(predicate))) -
                    m_data.rbegin());
        }
        template <typename ESCallback>
        constexpr Array<T> filter(ESCallback &&predicate)
        {
            auto filterRange = m_data | std::views::filter(basicAdaptor<bool>(std::forward<ESCallback>(predicate)));
            return Array<T>(filterRange.begin(), filterRange.end());
        }
        template <typename U, typename ESCallback>
        constexpr Array<U> map(ESCallback &&mapFn)
        {
            auto mapRange = m_data | std::views::transform(basicAdaptor<U>(std::forward<ESCallback>(mapFn)));
            return Array<U>(mapRange.begin(), mapRange.end());
        }
        template <typename ESCallback>
        constexpr T reduce(ESCallback &&reduceFn)
        {
            if (length <= 0) [[unlikely]]
                throw std::logic_error{"TypeError: Reduce of empty array with no initial value"};
            else if (length == 1) [[unlikely]]
                return m_data[0];
            return std::accumulate(
                m_data.begin() + 1,
                m_data.end(),
                m_data.front(),
                reduceAdaptor<T, false>(std::forward<ESCallback>(reduceFn)));
        }
        template <typename U, typename ESCallback>
        constexpr U reduce(ESCallback &&reduceFn, U initialValue)
        {
            if (length <= 0) [[unlikely]]
                return std::forward<U>(initialValue);
            return std::accumulate(
                m_data.begin(),
                m_data.end(),
                std::forward<U>(initialValue),
                reduceAdaptor<U, true>(std::forward<ESCallback>(reduceFn)));
        }
        template <typename ESCallback>
        constexpr T reduceRight(ESCallback &&reduceFn)
        {
            if (length <= 0) [[unlikely]]
                throw std::logic_error{"TypeError: Reduce of empty array with no initial value"};
            else if (length == 1) [[unlikely]]
                return m_data[0];
            return std::accumulate(
                m_data.rbegin() + 1,
                m_data.rend(),
                m_data.back(),
                reduceAdaptor<T, false, true>(std::forward<ESCallback>(reduceFn)));
        }
        template <typename U, typename ESCallback>
        constexpr U reduceRight(ESCallback &&reduceFn, U initialValue)
        {
            if (length <= 0) [[unlikely]]
                return std::forward<U>(initialValue);
            return std::accumulate(
                m_data.rbegin(),
                m_data.rend(),
                std::forward<U>(initialValue),
                reduceAdaptor<U, true, true>(std::forward<ESCallback>(reduceFn)));
        }
        constexpr Array<T> concat(const Array<T> &otherArray) const
        {
            Array<T> concatArray(length + otherArray.length);
            std::copy(m_data.begin(), m_data.end(), concatArray.m_data.begin());
            std::copy(otherArray.m_data.begin(), otherArray.m_data.end(), concatArray.m_data.begin() + length);
            return concatArray;
        }
        constexpr Array<T> &fill(const T &value, index_type start = 0, std::optional<index_type> _end = std::nullopt) noexcept
        {
            if (start >= length) [[unlikely]]
                return *this;
            index_type end = _end.value_or(length);
            (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
            (end < 0 && end > -length) && (end += length) || end > length && (end = length);
            if (end <= start) [[unlikely]]
                return *this;
            std::fill_n(m_data.begin() + start, end - start, value);
            return *this;
        }
        constexpr Array<T> slice(index_type start = 0, std::optional<index_type> _end = std::nullopt) const
        {
            if (start >= length) [[unlikely]]
                return {};
            index_type end = _end.value_or(length);
            (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
            (end < 0 && end > -length) && (end += length) || end > length && (end = length);
            if (end <= start) [[unlikely]]
                return {};
            auto sliceRange = m_data | std::views::drop(start) | std::views::take(end - start);
            return Array<T>(sliceRange.begin(), sliceRange.end());
        }
        constexpr Array<T> &copyWithin(index_type target, index_type start, std::optional<index_type> _end = std::nullopt) noexcept
        {
            if (target >= length || start >= length) [[unlikely]]
                return *this;
            index_type end = _end.value_or(length);
            (target < 0 && target > -length) && (target += length) || target < 0 && (target = 0);
            (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
            (end < 0 && end > -length) && (end += length) || end > length && (end = length);
            if (end <= start) [[unlikely]]
                return *this;
            index_type copyCount = std::min(end - start, end - target);
            if (target > start)
                std::copy_backward(m_data.begin() + start, m_data.begin() + start + copyCount, m_data.begin() + target + copyCount);
            else
                std::copy(m_data.begin() + start, m_data.begin() + start + copyCount, m_data.begin() + target);
            return *this;
        }
        constexpr Array<T> splice(index_type start, std::optional<index_type> _deleteCount = std::nullopt, std::initializer_list<T> newElements = {})
        {
            if (start >= length) [[unlikely]]
                return {};
            start = std::max((start + length) % length, 0LL);
            index_type deleteCount = std::clamp(_deleteCount.value_or(length - start), 0LL, length - start);
            index_type newElementsSize = static_cast<index_type>(newElements.size());
            Array<T> spliceArray(deleteCount);
            if (deleteCount > 0)
            {
                std::move(m_data.begin() + start, m_data.begin() + start + deleteCount, spliceArray.m_data.begin());
                m_data.erase(m_data.begin() + start, m_data.begin() + start + deleteCount);
            }
            m_data.insert(m_data.begin() + start, newElements);
            const_cast<index_type &>(length) = length - deleteCount + newElementsSize;
            return spliceArray;
        }
        constexpr Array<T> &reverse()
        {
            std::reverse(m_data.begin(), m_data.end());
            return *this;
        }
        constexpr Array<T> &sort()
        {
            std::sort(m_data.begin(), m_data.end());
            return *this;
        }
        template <typename ESCallback>
        constexpr Array<T> &sort(ESCallback &&compareFn)
        {
            std::sort(m_data.begin(), m_data.end(), [&](const T &a, const T &b)
                      { return std::forward<ESCallback>(compareFn)(a, b) < 0; });
            return *this;
        }
        //////////////////////////////////Operator
        constexpr T &operator[](index_type index)
        {
            if (index >= length) [[unlikely]]
            {
                m_data.resize(index + 1);
                const_cast<index_type &>(length) = index + 1;
            }
            return m_data[index];
        }
        constexpr Array<T> &operator=(std::initializer_list<T> elements)
        {
            if (elements.size() > std::numeric_limits<index_type>::max()) [[unlikely]]
                throw std::overflow_error("Array length is too large!");
            m_data.assign(elements);
            const_cast<index_type &>(length) = static_cast<index_type>(elements.size());
            return *this;
        }
        constexpr Array<T> &operator=(Array<T> &&other) noexcept
        {
            if (this == &other) [[unlikely]]
                return *this;
            m_data = std::move(other.m_data);
            const_cast<index_type &>(length) = std::exchange(const_cast<index_type &>(other.length), 0);
            return *this;
        }
        friend constexpr std::ostream &operator<<(std::ostream &os, const Array<T> &array)
        {
            os << "\n(" << array.length << ") [";
            index_type crtIndex = 0;
            for (const T &val : array.m_data)
                os << val << (++crtIndex == array.length ? "" : ", ");
            os << "]\n";
            return os;
        }
    };
} // CES
