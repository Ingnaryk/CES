#ifndef _STDARRAY_H_
#define _STDARRAY_H_

#include <list>

#include "any.h"

template <typename T>
class StdArray
{
private:
    //////////////////////////////////Wrapped data
    std::list<T> data{};

public:
    //////////////////////////////////Variadic templates functions' end
    template <typename First>
        requires(std::is_same_v<First, T> || std::is_same_v<T, any>)
    size_t push(const First &element)
    {
        data.push_back(element);
        *const_cast<size_t *>(&length) = length + 1;
        return length;
    }
    template <typename First>
        requires(std::is_same_v<First, T> || std::is_same_v<T, any>)
    size_t unshift(const First &element)
    {
        data.push_front(element);
        *const_cast<size_t *>(&length) = length + 1;
        return length;
    }
    //////////////////////////////////ES Property
    const size_t length{0};
    //////////////////////////////////Constructor
    StdArray(size_t size) : data(size), length{size} {}
    StdArray(const std::initializer_list<T> &elements) : length{elements.size()}
    {
        data.insert(data.end(), elements);
    }
    StdArray(const StdArray &otherArray) : data{otherArray.data}, length{otherArray.length} {}
    StdArray(StdArray &&rvArray) : length{rvArray.length}, data{rvArray.data} {}
    //////////////////////////////////ES Method
    template <typename First, typename... Rest>
    size_t push(const First &element, const Rest &...elements)
    {
        push(element);
        push(elements...);
        return length;
    }
    template <typename First, typename... Rest>
    size_t unshift(const First &element, const Rest &...elements)
    {
        unshift(elements...);
        unshift(element);
        return length;
    }
    any pop()
    {
        if (length <= 0)
            return undefined;
        T last = std::move(data.back());
        data.pop_back();
        *const_cast<size_t *>(&length) = length - 1;
        return last;
    }
    any shift()
    {
        if (length <= 0)
            return undefined;
        T first = std::move(data.front());
        data.pop_front();
        *const_cast<size_t *>(&length) = length - 1;
        return first;
    }
    any at(ptrdiff_t index) const
    {
        index < 0 && (index += length);
        if (index < 0 || index >= length)
            return undefined;
        auto itor = data.cbegin();
        advance(itor, index);
        return *itor;
    }
    ptrdiff_t indexOf(const T &value, ptrdiff_t fromIndex = 0) const
    {
        (fromIndex < 0 && fromIndex > -length) && (fromIndex += length) || fromIndex < 0 && (fromIndex = 0);
        auto itor = data.cbegin();
        advance(itor, fromIndex);
        for (; itor != data.cend(); itor++, fromIndex++)
        {
            if (*itor == value)
                return fromIndex;
        }
        return -1;
    }
    ptrdiff_t lastIndexOf(const T &value, ptrdiff_t fromIndex = -1) const
    {
        (fromIndex < 0 && fromIndex > -length) && (fromIndex += length) || fromIndex >= length && (fromIndex = length - 1);
        auto itor = data.crbegin();
        advance(itor, length - 1 - fromIndex);
        for (; itor != data.crend(); itor++, fromIndex--)
        {
            if (*itor == value)
                return fromIndex;
        }
        return -1;
    }
    bool includes(const T &value) const
    {
        return std::find(data.cbegin(), data.cend(), value) != data.cend();
    }
    std::string join(const char *seperator = ",") const
    {
        std::stringstream ss;
        size_t crtIndex = 0;
        for (const T &val : data)
        {
            ss << val << (crtIndex++ == length - 1 ? "" : seperator);
        }
        return ss.str();
    }
    void forEach(const std::function<void(T, size_t, std::reference_wrapper<StdArray<T>>)> &callbackFn)
    {
        size_t crtIndex = 0;
        for (T val : data)
            callbackFn(val, crtIndex++, std::ref(*this));
    }
    bool some(const std::function<bool(T, size_t, std::reference_wrapper<StdArray<T>>)> &predicate)
    {
        size_t crtIndex = 0;
        for (T val : data)
        {
            if (predicate(val, crtIndex++, std::ref(*this)))
                return true;
        }
        return false;
    }
    bool every(const std::function<bool(T, size_t, std::reference_wrapper<StdArray<T>>)> &predicate)
    {
        size_t crtIndex = 0;
        for (T val : data)
        {
            if (!predicate(val, crtIndex++, std::ref(*this)))
                return false;
        }
        return true;
    }
    any find(const std::function<bool(T, size_t, std::reference_wrapper<StdArray<T>>)> &predicate)
    {
        size_t crtIndex = 0;
        for (auto itor = data.cbegin(); itor != data.cend(); itor++)
        {
            if (predicate(*itor, crtIndex++, std::ref(*this)))
                return *itor;
        }
        return undefined;
    }
    any findLast(const std::function<bool(T, size_t, std::reference_wrapper<StdArray<T>>)> &predicate)
    {
        size_t crtIndex = length - 1;
        for (auto itor = data.crbegin(); itor != data.crend(); itor++)
        {
            if (predicate(*itor, crtIndex--, std::ref(*this)))
                return *itor;
        }
        return undefined;
    }
    ptrdiff_t findIndex(const std::function<bool(T, size_t, std::reference_wrapper<StdArray<T>>)> &predicate)
    {
        size_t crtIndex = 0;
        for (auto itor = data.cbegin(); itor != data.cend(); itor++)
        {
            if (predicate(*itor, crtIndex++, std::ref(*this)))
                return --crtIndex;
        }
        return -1;
    }
    ptrdiff_t findLastIndex(const std::function<bool(T, size_t, std::reference_wrapper<StdArray<T>>)> &predicate)
    {
        size_t crtIndex = length - 1;
        for (auto itor = data.crbegin(); itor != data.crend(); itor++)
        {
            if (predicate(*itor, crtIndex--, std::ref(*this)))
                return ++crtIndex;
        }
        return -1;
    }
    T reduce(const std::function<T(T, T, size_t, std::reference_wrapper<StdArray<T>>)> &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
    {
        if (length == 0)
            return undefined;
        auto itor = data.cbegin();
        size_t crtIndex = 0;
        T accumulator;
        if (_initialValue.has_value())
            accumulator = *_initialValue;
        else
        {
            accumulator = data.front();
            itor++;
            crtIndex++;
        }
        for (; itor != data.cend(); itor++)
        {
            accumulator = callbackFn(accumulator, *itor, crtIndex++, std::ref(*this));
        }
        return accumulator;
    }
    T reduceRight(const std::function<T(T, T, size_t, std::reference_wrapper<StdArray<T>>)> &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
    {
        if (length == 0)
            return undefined;
        auto itor = data.crbegin();
        size_t crtIndex = length - 1;
        T accumulator;
        if (_initialValue.has_value())
            accumulator = *_initialValue;
        else
        {
            accumulator = data.back();
            itor++;
            crtIndex--;
        }
        for (; itor != data.crend(); itor++)
        {
            accumulator = callbackFn(accumulator, *itor, crtIndex--, std::ref(*this));
        }
        return accumulator;
    }
    StdArray<T> filter(const std::function<bool(T, size_t, std::reference_wrapper<StdArray<T>>)> &predicate)
    {
        StdArray<T> filterArray(0);
        size_t crtIndex = 0;
        for (T val : data)
        {
            if (predicate(val, crtIndex++, std::ref(*this)))
                filterArray.push(val);
        }
        return filterArray;
    }
    StdArray<any> map(const std::function<any(T, size_t, std::reference_wrapper<StdArray<T>>)> &mapFn)
    {
        StdArray<any> mapArray(0);
        size_t crtIndex = 0;
        for (T val : data)
            mapArray.push(std::move(mapFn(val, crtIndex++, std::ref(*this))));
        return mapArray;
    }
    StdArray<T> concat(const StdArray<T> &otherArray) const
    {
        StdArray<T> concatArray{*this};
        for (const T &ele : otherArray.data)
            concatArray.push(ele);
        return concatArray;
    }
    StdArray<T> slice(ptrdiff_t start, std::optional<ptrdiff_t> _end = std::nullopt) const
    {
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length);
        end > length && (end = length);
        if (start >= length || end <= start)
            return StdArray<T>(0);
        StdArray<T> sliceArray(end - start);
        auto sitor = data.cbegin();
        auto eitor = data.cbegin();
        advance(sitor, start);
        advance(eitor, end);
        copy(sitor, eitor, sliceArray.data.begin());
        return sliceArray;
    }
    StdArray<T> splice(ptrdiff_t start, std::optional<size_t> _deleteCount = std::nullopt, const std::initializer_list<T> &newElements = {})
    {
        StdArray<T> spliceArray(0);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        if (start >= length)
            return spliceArray;
        size_t deleteCount = (_deleteCount.has_value() ? *_deleteCount : length - start);
        deleteCount > length - start && (deleteCount = length - start);
        deleteCount < 0 && (deleteCount = 0);
        auto sitor = data.begin();
        auto ditor = data.begin();
        advance(sitor, start);
        advance(ditor, start + deleteCount);
        spliceArray.data.splice(spliceArray.data.begin(), data, sitor, ditor);
        *const_cast<size_t *>(&spliceArray.length) = deleteCount;
        sitor = data.begin();
        advance(sitor, start);
        data.insert(sitor, newElements);
        *const_cast<size_t *>(&length) = length - deleteCount + newElements.size();
        return spliceArray;
    }
    StdArray<T> &reverse()
    {
        data.reverse();
        return *this;
    }
    StdArray<T> &sort(const std::optional<std::function<ptrdiff_t(T, T)>> &_compareFn = std::nullopt)
    {
        if (!_compareFn.has_value())
            data.sort();
        else
        {
            auto &compareFn = *_compareFn;
            data.sort([&compareFn](T a, T b)
                      { return compareFn(a, b) < 0; });
        }
        return *this;
    }
    StdArray<T> &copyWithin(ptrdiff_t target, ptrdiff_t start, std::optional<ptrdiff_t> _end = std::nullopt)
    {
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (target < 0 && target > -length) && (target += length) || target < 0 && (target = 0);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length);
        end > length && (end = length);
        if (target >= length || start >= length || end <= start)
            return *this;
        size_t des = end - start, det = end - target;
        auto titor = data.begin();
        auto sitor = data.begin();
        advance(titor, target);
        advance(sitor, start);
        copy_n(sitor, std::min(des, det), titor);
        return *this;
    }
    StdArray<T> &fill(const T &value, ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt)
    {
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length);
        end > length && (end = length);
        if (start >= length || end <= start)
            return *this;
        auto sitor = data.begin();
        advance(sitor, start);
        fill_n(sitor, end - start, value);
        return *this;
    }
    //////////////////////////////////Operator
    T &operator[](size_t index)
    {
        if (index >= length)
        {
            data.resize(index + 1);
            *const_cast<size_t *>(&length) = index + 1;
        }
        else if (index < 0)
            index = 0;
        auto itor = data.begin();
        advance(itor, index);
        return *itor;
    }
    StdArray<T> &operator=(const StdArray<T> &otherArray)
    {
        if (this != &otherArray)
        {
            std::list<T>(otherArray.data).swap(data);
            *const_cast<size_t *>(&length) = otherArray.length;
        }
        return *this;
    }
    StdArray<T> &operator=(StdArray<T> &&rvArray)
    {
        std::swap(const_cast<size_t &>(length), const_cast<size_t &>(rvArray.length));
        data.swap(rvArray.data);
        return *this;
    }
    friend std::ostream &operator<<(std::ostream &os, const StdArray<T> &array)
    {
        if (array.length > 1)
            os << "\n(" << array.length << ") ";
        os << "[";
        size_t crtIndex = 0;
        for (const T &val : array.data)
        {
            os << val << (crtIndex++ == array.length - 1 ? "" : ", ");
        }
        os << "]\n";
        return os;
    }
};

#endif // _STDARRAY_H_
