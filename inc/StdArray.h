#ifndef _STDARRAY_H_
#define _STDARRAY_H_

#include <list>

#include "Any.h"

template <typename T>
class StdArray
{
    //////////////////////////////////Utilities
    template <typename U>
    using CallbackF_Basic = std::variant<std::function<U()>,
                                         std::function<U(T)>,
                                         std::function<U(T, size_t)>,
                                         std::function<U(T, size_t, StdArray &)>>;
    using CallbackF_Reduce = std::variant<std::function<T(T, T)>,
                                          std::function<T(T, T, size_t)>,
                                          std::function<T(T, T, size_t, StdArray &)>>;
    template <typename newT>
    std::function<newT(T, size_t, StdArray &)> convert_basic(const CallbackF_Basic<newT> &fn)
    {
        std::function<newT(T, size_t, StdArray &)> f;
        switch (fn.index())
        {
        case 0:
            f = [&fn](T, size_t, StdArray &)
            { return std::get<0>(fn)(); };
            break;
        case 1:
            f = [&fn](T value, size_t, StdArray &)
            { return std::get<1>(fn)(value); };
            break;
        case 2:
            f = [&fn](T value, size_t index, StdArray &)
            { return std::get<2>(fn)(value, index); };
            break;
        case 3:
            f = [&fn](T value, size_t index, StdArray &thisArray)
            { return std::get<3>(fn)(value, index, thisArray); };
            break;
        }
        return f;
    }
    std::function<T(T, T, size_t, StdArray &)> convert_reduce(const CallbackF_Reduce &fn)
    {
        std::function<T(T, T, size_t, StdArray &)> f;
        switch (fn.index())
        {
        case 0:
            f = [&fn](T value1, T value2, size_t, StdArray &)
            { return std::get<0>(fn)(value1, value2); };
            break;
        case 1:
            f = [&fn](T value1, T value2, size_t crtIndex, StdArray &)
            { return std::get<1>(fn)(value1, value2, crtIndex); };
            break;
        case 2:
            f = [&fn](T value1, T value2, size_t crtIndex, StdArray &thisArray)
            { return std::get<2>(fn)(value1, value2, crtIndex, thisArray); };
            break;
        }
        return f;
    }

private:
    //////////////////////////////////Wrapped data
    std::list<T> data{};

public:
    //////////////////////////////////Variadic templates functions' end
    template <typename Arg>
        requires(std::is_same_v<Arg, T> || std::is_same_v<T, Any>)
    size_t push(const Arg &element)
    {
        data.push_back(element);
        *const_cast<size_t *>(&length) = length + 1;
        return length;
    }
    template <typename Arg>
        requires(std::is_same_v<Arg, T> || std::is_same_v<T, Any>)
    size_t unshift(const Arg &element)
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
    StdArray(StdArray &&rvArray) noexcept
    {
        std::swap(*const_cast<size_t *>(&length), *const_cast<size_t *>(&rvArray.length));
        data.swap(rvArray.data);
    }
    //////////////////////////////////ES Method
    template <typename Arg, typename... RestArgs>
    size_t push(const Arg &element, RestArgs const &...elements)
    {
        push(element);
        push(elements...);
        return length;
    }
    template <typename Arg, typename... RestArgs>
    size_t unshift(const Arg &element, RestArgs const &...elements)
    {
        unshift(elements...);
        unshift(element);
        return length;
    }
    Any pop()
    {
        if (length <= 0)
            return undefined;
        T last = data.back();
        data.pop_back();
        *const_cast<size_t *>(&length) = length - 1;
        return last;
    }
    Any shift()
    {
        if (length <= 0)
            return undefined;
        T first = data.front();
        data.pop_front();
        *const_cast<size_t *>(&length) = length - 1;
        return first;
    }
    Any at(ptrdiff_t index) const
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
    void forEach(const CallbackF_Basic<void> &callbackFn)
    {
        auto f = convert_basic<void>(callbackFn);
        size_t crtIndex = 0;
        for (T val : data)
            f(val, crtIndex++, *this);
    }
    bool some(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        size_t crtIndex = 0;
        for (T val : data)
        {
            if (f(val, crtIndex++, *this))
                return true;
        }
        return false;
    }
    bool every(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        size_t crtIndex = 0;
        for (T val : data)
        {
            if (!f(val, crtIndex++, *this))
                return false;
        }
        return true;
    }
    Any find(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        size_t crtIndex = 0;
        for (auto itor = data.cbegin(); itor != data.cend(); itor++)
        {
            if (f(*itor, crtIndex++, *this))
                return *itor;
        }
        return undefined;
    }
    Any findLast(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        size_t crtIndex = length - 1;
        for (auto itor = data.crbegin(); itor != data.crend(); itor++)
        {
            if (f(*itor, crtIndex--, *this))
                return *itor;
        }
        return undefined;
    }
    ptrdiff_t findIndex(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        size_t crtIndex = 0;
        for (auto itor = data.cbegin(); itor != data.cend(); itor++)
        {
            if (f(*itor, crtIndex++, *this))
                return --crtIndex;
        }
        return -1;
    }
    ptrdiff_t findLastIndex(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        size_t crtIndex = length - 1;
        for (auto itor = data.crbegin(); itor != data.crend(); itor++)
        {
            if (f(*itor, crtIndex--, *this))
                return ++crtIndex;
        }
        return -1;
    }
    T reduce(const CallbackF_Reduce &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
    {
        if (length == 0)
            return undefined;
        auto f = convert_reduce(callbackFn);
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
            accumulator = f(accumulator, *itor, crtIndex++, *this);
        }
        return accumulator;
    }
    T reduceRight(const CallbackF_Reduce &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
    {
        if (length == 0)
            return undefined;
        auto f = convert_reduce(callbackFn);
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
            accumulator = f(accumulator, *itor, crtIndex--, *this);
        }
        return accumulator;
    }
    StdArray<T> filter(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        StdArray<T> filterArray(0);
        size_t crtIndex = 0;
        for (T val : data)
        {
            if (f(val, crtIndex++, *this))
                filterArray.push(val);
        }
        return filterArray;
    }
    StdArray<Any> map(const CallbackF_Basic<Any> &mapFn)
    {
        auto f = convert_basic<Any>(mapFn);
        StdArray<Any> mapArray(0);
        size_t crtIndex = 0;
        for (T val : data)
            mapArray.push(std::move(f(val, crtIndex++, *this)));
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
    StdArray<T> &operator=(const StdArray<T> &newArray)
    {
        if (this != &newArray)
        {
            std::list<T>(newArray.data).swap(data);
            *const_cast<size_t *>(&length) = newArray.length;
        }
        return *this;
    }
    StdArray<T> &operator=(StdArray<T> &&newArray)
    {
        *const_cast<size_t *>(&length) = std::__exchange(*const_cast<size_t *>(&newArray.length), 0);
        data = std::__exchange(newArray.data, std::list<T>{});
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