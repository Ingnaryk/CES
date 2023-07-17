#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <vector>
#include <ranges>
#include <numeric>
#include <cstdint>

#include "CES/any.h"

namespace CES
{
    template <typename T>
    class Array
    {
    private:
        //////////////////////////////////Internal usage
        enum class Direction
        {
            Forward,
            Backward
        };
        template <typename R, Direction dir = Direction::Forward, typename ESCallback>
        constexpr auto basicAdaptor(ESCallback &&func)
        {
            constexpr bool reverse = dir == Direction::Backward;
            return [&, index = reverse ? length - 1 : 0LL](const T &value) mutable -> R
            {
                if constexpr (std::is_invocable_r_v<R, ESCallback, T>)
                    return func(value);
                else if constexpr (std::is_invocable_r_v<R, ESCallback, T, ptrdiff_t>)
                    return func(value, reverse ? index-- : index++);
                else if constexpr (std::is_invocable_r_v<R, ESCallback, T, ptrdiff_t, Array<T> &>)
                    return func(value, reverse ? index-- : index++, *this);
            };
        }
        enum class ReduceFlag
        {
            Initial,
            NonInitial
        };
        template <typename R, ReduceFlag flag, Direction dir = Direction::Forward, typename ESCallback>
        constexpr auto reduceAdaptor(ESCallback &&func)
        {
            constexpr bool reverse = dir == Direction::Backward;
            constexpr bool hasInitialValue = flag == ReduceFlag::Initial;
            ptrdiff_t startIndex = reverse ? (hasInitialValue ? length - 1 : length - 2)
                                           : (hasInitialValue ? 0LL : 1LL);
            using Init = std::conditional_t<hasInitialValue, R, T>;
            return [&, index = startIndex](const Init &initial, const T &value) mutable -> Init
            {
                if constexpr (std::is_invocable_r_v<Init, ESCallback, Init, T>)
                    return func(initial, value);
                else if constexpr (std::is_invocable_r_v<Init, ESCallback, Init, T, ptrdiff_t>)
                    return func(initial, value, reverse ? index-- : index++);
                else if constexpr (std::is_invocable_r_v<Init, ESCallback, Init, T, ptrdiff_t, Array<T> &>)
                    return func(initial, value, reverse ? index-- : index++, *this);
            };
        }
        //////////////////////////////////Wrapped data
        std::vector<T> data{};

    public:
        //////////////////////////////////ES Property
        const ptrdiff_t length{0};
        //////////////////////////////////Constructor
        Array() noexcept = default;
        explicit Array(ptrdiff_t size) : data(size), length{size} {}
        Array(std::initializer_list<T> elements) : data{elements}
        {
            if (elements.size() > INT64_MAX)
                throw std::overflow_error("Array length is too large!");
            const_cast<ptrdiff_t &>(length) = (ptrdiff_t)elements.size();
        }
        template <typename Iter>
        Array(Iter begin, Iter end) : data{begin, end}
        {
            auto size = std::ranges::distance(begin, end);
            if (size > INT64_MAX)
                throw std::overflow_error("Array length is too large!");
            const_cast<ptrdiff_t &>(length) = (ptrdiff_t)size;
        }
        Array(const Array &other) : data{other.data}, length{other.length} {}
        Array(Array &&other) noexcept : data{std::move(other.data)}, length{std::exchange(const_cast<ptrdiff_t &>(other.length), 0)} {}
        //////////////////////////////////ES Method
        template <typename... Args>
        constexpr ptrdiff_t push(Args &&...elements)
        {
            data.insert(data.end(), std::initializer_list<T>{std::forward<Args>(elements)...});
            const_cast<ptrdiff_t &>(length) = data.size();
            return length;
        }
        template <typename... Args>
        constexpr ptrdiff_t unshift(Args &&...elements)
        {
            data.insert(data.begin(), std::initializer_list<T>{std::forward<Args>(elements)...});
            const_cast<ptrdiff_t &>(length) = data.size();
            return length;
        }
        constexpr T pop()
        {
            T back = data.back();
            data.pop_back();
            const_cast<ptrdiff_t &>(length) -= 1;
            return back;
        }
        constexpr T shift()
        {
            T front = data.front();
            data.erase(data.begin());
            const_cast<ptrdiff_t &>(length) -= 1;
            return front;
        }
        constexpr const T &at(ptrdiff_t index) const
        {
            index < 0 && (index = (index + length) % length); // c++ feature: [-length, -1] % length returns itself!
            return data.at(index);
        }
        constexpr ptrdiff_t indexOf(const T &value, ptrdiff_t fromIndex = 0) const noexcept
        {
            if (fromIndex >= length) [[unlikely]]
                return -1;
            (fromIndex < 0 && fromIndex > -length) && (fromIndex += length) || fromIndex < 0 && (fromIndex = 0);
            auto findRange = data | std::views::drop(fromIndex);
            ptrdiff_t index = std::ranges::find(findRange, value) - findRange.begin();
            return index == findRange.size() ? -1 : fromIndex + index;
        }
        constexpr ptrdiff_t lastIndexOf(const T &value, ptrdiff_t fromIndex = -1) const noexcept
        {
            (fromIndex < 0 && fromIndex >= -length) && (fromIndex += length) || fromIndex >= length && (fromIndex = length - 1);
            if (fromIndex < 0) [[unlikely]]
                return -1;
            auto findRange = data | std::views::take(fromIndex + 1) | std::views::reverse;
            ptrdiff_t index = std::ranges::find(findRange, value) - findRange.begin();
            return index == findRange.size() ? -1 : fromIndex - index;
        }
        constexpr bool includes(const T &value) const noexcept
        {
            return std::ranges::find(data, value) != data.end();
        }
        constexpr std::string join(const char *separator = ",", std::string (*formatter)(const T &) = to_string) const noexcept
        {
            std::string s;
            for (ptrdiff_t i = 0; i < length; ++i)
                s.operator+=(formatter(data[i])).operator+=((i == length - 1 ? "" : separator));
            return s;
        }
        template <typename ESCallback>
        constexpr void forEach(ESCallback &&callbackFn)
        {
            std::for_each(data.begin(), data.end(), basicAdaptor<void>(std::forward<ESCallback>(callbackFn)));
        }
        template <typename ESCallback>
        constexpr bool some(ESCallback &&predicate)
        {
            return std::any_of(data.begin(), data.end(), basicAdaptor<bool>(std::forward<ESCallback>(predicate)));
        }
        template <typename ESCallback>
        constexpr bool every(ESCallback &&predicate)
        {
            return std::all_of(data.begin(), data.end(), basicAdaptor<bool>(std::forward<ESCallback>(predicate)));
        }
        template <typename ESCallback>
        constexpr any find(ESCallback &&predicate)
        {
            auto value = std::ranges::find_if(data, basicAdaptor<bool>(std::forward<ESCallback>(predicate)));
            return value == data.end() ? undefined : *value;
        }
        template <typename ESCallback>
        constexpr any findLast(ESCallback &&predicate)
        {
            auto value = std::ranges::find_if(data.rbegin(), data.rend(), basicAdaptor<bool>(std::forward<ESCallback>(predicate)));
            return value == data.rend() ? undefined : *value;
        }
        template <typename ESCallback>
        constexpr ptrdiff_t findIndex(ESCallback &&predicate)
        {
            ptrdiff_t index = std::ranges::find_if(data, basicAdaptor<bool>(std::forward<ESCallback>(predicate))) - data.begin();
            return index == length ? -1 : index;
        }
        template <typename ESCallback>
        constexpr ptrdiff_t findLastIndex(ESCallback &&predicate)
        {
            return length - 1 -
                   (std::ranges::find_if(
                        data.rbegin(),
                        data.rend(),
                        basicAdaptor<bool, Direction::Backward>(std::forward<ESCallback>(predicate))) -
                    data.rbegin());
        }
        template <typename ESCallback>
        constexpr Array<T> filter(ESCallback &&predicate)
        {
            auto filterRange = data | std::views::filter(basicAdaptor<bool>(std::forward<ESCallback>(predicate)));
            return Array<T>(filterRange.begin(), filterRange.end());
        }
        template <typename U, typename ESCallback>
        constexpr Array<U> map(ESCallback &&mapFn)
        {
            auto mapRange = data | std::views::transform(basicAdaptor<U>(std::forward<ESCallback>(mapFn)));
            return Array<U>(mapRange.begin(), mapRange.end());
        }
        template <typename ESCallback>
        constexpr T reduce(ESCallback &&reduceFn)
        {
            if (length <= 0) [[unlikely]]
                throw std::logic_error{"TypeError: Reduce of empty array with no initial value"};
            else if (length == 1) [[unlikely]]
                return data[0];
            return std::accumulate(
                data.begin() + 1,
                data.end(),
                data.front(),
                reduceAdaptor<T, ReduceFlag::NonInitial>(std::forward<ESCallback>(reduceFn)));
        }
        template <typename U, typename ESCallback>
        constexpr U reduce(ESCallback &&reduceFn, U initialValue)
        {
            if (length <= 0) [[unlikely]]
                return std::forward<U>(initialValue);
            return std::accumulate(
                data.begin(),
                data.end(),
                std::forward<U>(initialValue),
                reduceAdaptor<U, ReduceFlag::Initial>(std::forward<ESCallback>(reduceFn)));
        }
        template <typename ESCallback>
        constexpr T reduceRight(ESCallback &&reduceFn)
        {
            if (length <= 0) [[unlikely]]
                throw std::logic_error{"TypeError: Reduce of empty array with no initial value"};
            else if (length == 1) [[unlikely]]
                return data[0];
            return std::accumulate(
                data.rbegin() + 1,
                data.rend(),
                data.back(),
                reduceAdaptor<T, ReduceFlag::NonInitial, Direction::Backward>(std::forward<ESCallback>(reduceFn)));
        }
        template <typename U, typename ESCallback>
        constexpr U reduceRight(ESCallback &&reduceFn, U initialValue)
        {
            if (length <= 0) [[unlikely]]
                return std::forward<U>(initialValue);
            return std::accumulate(
                data.rbegin(),
                data.rend(),
                std::forward<U>(initialValue),
                reduceAdaptor<U, ReduceFlag::NonInitial, Direction::Backward>(std::forward<ESCallback>(reduceFn)));
        }
        constexpr Array<T> concat(const Array<T> &otherArray) const
        {
            Array<T> concatArray(length + otherArray.length);
            std::copy(data.begin(), data.end(), concatArray.data.begin());
            std::copy(otherArray.data.begin(), otherArray.data.end(), concatArray.data.begin() + length);
            return concatArray;
        }
        constexpr Array<T> &fill(const T &value, ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt)
        {
            if (start >= length) [[unlikely]]
                return *this;
            ptrdiff_t end = _end.value_or(length);
            (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
            (end < 0 && end > -length) && (end += length) || end > length && (end = length);
            if (end <= start) [[unlikely]]
                return *this;
            std::fill_n(data.begin() + start, end - start, value);
            return *this;
        }
        constexpr Array<T> slice(ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt) const
        {
            if (start >= length) [[unlikely]]
                return {};
            ptrdiff_t end = _end.value_or(length);
            (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
            (end < 0 && end > -length) && (end += length) || end > length && (end = length);
            if (end <= start) [[unlikely]]
                return {};
            auto sliceRange = data | std::views::drop(start) | std::views::take(end - start);
            return Array<T>(sliceRange.begin(), sliceRange.end());
        }
        constexpr Array<T> &copyWithin(ptrdiff_t target, ptrdiff_t start, std::optional<ptrdiff_t> _end = std::nullopt)
        {
            if (target >= length || start >= length) [[unlikely]]
                return *this;
            ptrdiff_t end = _end.value_or(length);
            (target < 0 && target > -length) && (target += length) || target < 0 && (target = 0);
            (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
            (end < 0 && end > -length) && (end += length) || end > length && (end = length);
            if (end <= start) [[unlikely]]
                return *this;
            ptrdiff_t copyCount = std::min(end - start, end - target);
            if (target > start)
                std::copy_backward(data.begin() + start, data.begin() + start + copyCount, data.begin() + target + copyCount);
            else
                std::copy(data.begin() + start, data.begin() + start + copyCount, data.begin() + target);
            return *this;
        }
        constexpr Array<T> splice(ptrdiff_t start, std::optional<ptrdiff_t> _deleteCount = std::nullopt, std::initializer_list<T> newElements = {})
        {
            if (start >= length) [[unlikely]]
                return {};
            start = std::max((start + length) % length, 0LL);
            ptrdiff_t deleteCount = std::clamp(_deleteCount.value_or(length - start), 0LL, length - start);
            ptrdiff_t newElementsSize = (ptrdiff_t)newElements.size();
            Array<T> spliceArray(deleteCount);
            if (deleteCount > 0)
            {
                std::move(data.begin() + start, data.begin() + start + deleteCount, spliceArray.data.begin());
                data.erase(data.begin() + start, data.begin() + start + deleteCount);
            }
            data.insert(data.begin() + start, newElements);
            const_cast<ptrdiff_t &>(length) = length - deleteCount + newElementsSize;
            return spliceArray;
        }
        constexpr Array<T> &reverse()
        {
            std::reverse(data.begin(), data.end());
            return *this;
        }
        constexpr Array<T> &sort()
        {
            std::sort(data.begin(), data.end());
            return *this;
        }
        template <typename ESCallback>
        constexpr Array<T> &sort(ESCallback &&compareFn)
        {
            std::sort(data.begin(), data.end(), [&compareFn](const T &a, const T &b)
                      { return compareFn(a, b) < 0; });
            return *this;
        }
        //////////////////////////////////Operator
        constexpr T &operator[](ptrdiff_t index)
        {
            if (index >= length) [[unlikely]]
            {
                data.resize(index + 1);
                const_cast<ptrdiff_t &>(length) = index + 1;
            }
            else if (index < 0) [[unlikely]]
                index = 0;
            return data[index];
        }
        constexpr Array<T> &operator=(const Array<T> &otherArray)
        {
            if (this == &otherArray) [[unlikely]]
                return *this;
            data = otherArray.data;
            const_cast<ptrdiff_t &>(length) = otherArray.length;
            return *this;
        }
        constexpr Array<T> &operator=(Array<T> &&otherArray)
        {
            if (this == &otherArray) [[unlikely]]
                return *this;
            data = std::move(otherArray.data);
            const_cast<ptrdiff_t &>(length) = std::exchange(const_cast<ptrdiff_t &>(otherArray.length), 0);
            return *this;
        }
        friend constexpr std::ostream &operator<<(std::ostream &os, const Array<T> &array)
        {
            os << "\n(" << array.length << ") [";
            ptrdiff_t crtIndex = 0;
            for (const T &val : array.data)
                os << val << (++crtIndex == array.length ? "" : ", ");
            os << "]\n";
            return os;
        }
    };
} // CES

#endif // _ARRAY_H_
