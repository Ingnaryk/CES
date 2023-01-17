#ifndef _RAWARRAY_H_
#define _RAWARRAY_H_

#include <memory>

#include "Any.h"

template <typename T>
class RawArray
{
    //////////////////////////////////Utilities
    template <typename U>
    using CallbackF_Basic = std::variant<std::function<U()>,
                                         std::function<U(T)>,
                                         std::function<U(T, size_t)>,
                                         std::function<U(T, size_t, RawArray<T> &)>>;
    template <typename U>
    using CallbackF_Reduce = std::variant<std::function<U(T, T)>,
                                          std::function<U(T, T, size_t)>,
                                          std::function<U(T, T, size_t, RawArray<T> &)>>;
    template <typename newT>
    std::function<newT(T, size_t, RawArray<T> &)> convert_basic(const CallbackF_Basic<newT> &fn)
    {
        std::function<newT(T, size_t, RawArray<T> &)> f;
        switch (fn.index())
        {
        case 0:
            f = [&fn](T, size_t, RawArray<T> &)
            { return std::get<0>(fn)(); };
            break;
        case 1:
            f = [&fn](T value, size_t, RawArray<T> &)
            { return std::get<1>(fn)(value); };
            break;
        case 2:
            f = [&fn](T value, size_t index, RawArray<T> &)
            { return std::get<2>(fn)(value, index); };
            break;
        case 3:
            f = [&fn](T value, size_t index, RawArray<T> &thisArray)
            { return std::get<3>(fn)(value, index, thisArray); };
            break;
        }
        return f;
    }
    template <typename newT>
    std::function<newT(T, T, size_t, RawArray<T> &)> convert_reduce(const CallbackF_Reduce<newT> &fn)
    {
        std::function<newT(T, T, size_t, RawArray<T> &)> f;
        switch (fn.index())
        {
        case 0:
            f = [&fn](T value1, T value2, size_t, RawArray<T> &)
            { return std::get<0>(fn)(value1, value2); };
            break;
        case 1:
            f = [&fn](T value1, T value2, size_t crtIndex, RawArray<T> &)
            { return std::get<1>(fn)(value1, value2, crtIndex); };
            break;
        case 2:
            f = [&fn](T value1, T value2, size_t crtIndex, RawArray<T> &thisArray)
            { return std::get<2>(fn)(value1, value2, crtIndex, thisArray); };
            break;
        }
        return f;
    }

private:
    //////////////////////////////////Internal usage
    T *alloc(size_t n)
    {
        return static_cast<T *>(::operator new(n * sizeof(T)));
    }
    void dealloc(T *p) noexcept
    {
        ::operator delete(p);
    }
    template <typename... Args>
    void construct(T *p, Args &&...args)
    {
        ::new (p) T(std::forward<Args>(args)...);
    }
    void destroy(T *p) noexcept
    {
        p->~T();
    }
    void clear() noexcept
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (; _length > 0; _length--)
            destroy(data + _length - 1);
    }
    void swap(RawArray<T> &otherArray) noexcept
    {
        std::swap(capacity, otherArray.capacity);
        std::swap(*const_cast<size_t *>(&length), *const_cast<size_t *>(&otherArray.length));
        std::swap(data, otherArray.data);
    }
    template <typename Arg>
    void insertAt(size_t loc, const Arg &element)
    {
        if (length == capacity)
        {
            size_t newCapacity = (capacity ? capacity * 2 : 1);
            T *newData = alloc(newCapacity);
            for (size_t i = 0; i < length; i++)
                construct(newData + i, std::move(data[i]));
            capacity = newCapacity;
            data = newData;
        }
        for (ptrdiff_t i = length; i > loc; i--)
            construct(data + i, std::move(data[i - 1]));
        construct(data + loc, element);
        *const_cast<size_t *>(&length) = length + 1;
    }
    //////////////////////////////////Wrapped data
    size_t capacity{0};
    T *data{nullptr};

public:
    //////////////////////////////////Variadic templates functions' end
    template <typename Arg>
        requires(std::is_same_v<Arg, T> || std::is_same_v<T, Any>)
    size_t push(const Arg &element)
    {
        insertAt(length, std::move(element));
        return length;
    }
    template <typename Arg>
        requires(std::is_same_v<Arg, T> || std::is_same_v<T, Any>)
    size_t unshift(const Arg &element)
    {
        insertAt(0, std::move(element));
        return length;
    }
    //////////////////////////////////ES Property
    const size_t length{0};
    //////////////////////////////////Constructor
    explicit RawArray(size_t size) : capacity{size}, data{alloc(capacity)}
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (; _length < size; _length++)
            construct(data + _length); // call T's default constructor
    }
    RawArray(T *otherData, size_t size) : capacity{size}, data{alloc(capacity)}
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (; _length < size; _length++)
            construct(data + _length, std::move(otherData[_length]));
    }
    RawArray(const std::initializer_list<T> &elements) : capacity{elements.size()}, data{alloc(capacity)}
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (const auto &ele : elements)
            construct(data + _length++, std::move(ele));
    }
    RawArray(const RawArray<T> &otherArray) : capacity{otherArray.length}, data{alloc(capacity)}
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (; _length < otherArray.length; _length++)
            construct(data + _length, otherArray.data[_length]); // call T's copy constructor
    }
    RawArray(RawArray<T> &&rvArray) noexcept
    {
        swap(rvArray);
    }
    //////////////////////////////////ES Method
    template <typename Arg, typename... RestArgs>
    size_t push(const Arg &element, const RestArgs &...elements)
    {
        push(std::move(element));
        push(std::move(elements)...);
        return length;
    }
    template <typename Arg, typename... RestArgs>
    size_t unshift(const Arg &element, const RestArgs &...elements)
    {
        unshift(std::move(elements)...);
        unshift(std::move(element));
        return length;
    }
    Any pop()
    {
        if (length <= 0)
            return undefined;
        T last = data[length - 1];
        if (length < capacity / 2)
        {
            size_t newCapacity = capacity / 2;
            T *newData = alloc(newCapacity);
            for (size_t i = 0; i < length; i++)
                construct(newData + i, std::move(data[i]));
            capacity = newCapacity;
            data = newData;
        }
        destroy(data + length - 1);
        *const_cast<size_t *>(&length) = length - 1;
        return last;
    }
    Any shift()
    {
        if (length <= 0)
            return undefined;
        T first = data[0];
        if (length < capacity / 2)
        {
            size_t newCapacity = capacity / 2;
            T *newData = alloc(newCapacity);
            for (size_t i = 0; i < length; i++)
                construct(newData + i, std::move(data[i]));
            capacity = newCapacity;
            data = newData;
        }
        destroy(data + 0);
        for (size_t i = 0; i < length - 1; i++)
            construct(data + i, std::move(data[i + 1]));
        *const_cast<size_t *>(&length) = length - 1;
        return first;
    }
    Any at(ptrdiff_t index) const
    {
        index < 0 && (index += length);
        if (index < 0 || index >= length)
            return undefined;
        return data[index];
    }
    ptrdiff_t indexOf(const T &value, ptrdiff_t fromIndex = 0) const
    {
        (fromIndex < 0 && fromIndex > -length) && (fromIndex += length) || fromIndex < 0 && (fromIndex = 0);
        for (size_t i = fromIndex; i < length; i++)
        {
            if (data[i] == value)
                return i;
        }
        return -1;
    }
    ptrdiff_t lastIndexOf(const T &value, ptrdiff_t fromIndex = -1) const
    {
        (fromIndex < 0 && fromIndex > -length) && (fromIndex += length) || fromIndex >= length && (fromIndex = length - 1);
        for (size_t i = fromIndex; i >= 0; i--)
        {
            if (data[i] == value)
                return i;
        }
        return -1;
    }
    bool includes(const T &value) const
    {
        for (size_t i = 0; i < length; i++)
        {
            if (data[i] == value)
                return true;
        }
        return false;
    }
    std::string join(const char *separator = ",") const
    {
        std::stringstream ss;
        for (size_t i = 0; i < length; i++)
        {
            ss << data[i] << (i == length - 1 ? "" : separator);
        }
        return ss.str();
    }
    void forEach(const CallbackF_Basic<void> &callbackFn)
    {
        auto f = convert_basic<void>(callbackFn);
        for (size_t i = 0; i < length; i++)
            f(data[i], i, *this);
    }
    bool some(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        for (size_t i = 0; i < length; i++)
        {
            if (f(data[i], i, *this))
                return true;
        }
        return false;
    }
    bool every(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        for (size_t i = 0; i < length; i++)
        {
            if (!f(data[i], i, *this))
                return false;
        }
        return true;
    }
    Any find(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        for (size_t i = 0; i < length; i++)
        {
            if (f(data[i], i, *this))
                return data[i];
        }
        return undefined;
    }
    Any findLast(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        for (size_t i = length - 1; i >= 0; i--)
        {
            if (f(data[i], i, *this))
                return data[i];
        }
        return undefined;
    }
    ptrdiff_t findIndex(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        for (size_t i = 0; i < length; i++)
        {
            if (f(data[i], i, *this))
                return i;
        }
        return -1;
    }
    ptrdiff_t findLastIndex(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        for (size_t i = length - 1; i >= 0; i--)
        {
            if (f(data[i], i, *this))
                return i;
        }
        return -1;
    }
    T reduce(const CallbackF_Reduce<T> &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
    {
        if (length <= 0)
            return T{};
        auto f = convert_reduce<T>(callbackFn);
        ptrdiff_t crtIndex = 0;
        T accumulator;
        if (_initialValue.has_value())
            accumulator = *_initialValue;
        else
        {
            accumulator = data[crtIndex];
            crtIndex++;
        }
        for (ptrdiff_t i = crtIndex; i < length; i++)
            accumulator = f(accumulator, data[i], i, *this);
        return accumulator;
    }
    T reduceRight(const CallbackF_Reduce<T> &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
    {
        if (length <= 0)
            return T{};
        auto f = convert_reduce<T>(callbackFn);
        ptrdiff_t crtIndex = length - 1;
        T accumulator;
        if (_initialValue.has_value())
            accumulator = *_initialValue;
        else
        {
            accumulator = data[crtIndex];
            crtIndex--;
        }
        for (ptrdiff_t i = crtIndex; i >= 0; i--)
            accumulator = f(accumulator, data[i], i, *this);
        return accumulator;
    }
    RawArray<T> filter(const CallbackF_Basic<bool> &predicate)
    {
        auto f = convert_basic<bool>(predicate);
        RawArray<T> filterArray(0);
        size_t filterLength = 0;
        for (size_t i = 0; i < length; i++)
        {
            if (f(data[i], i, *this))
                filterArray.push(data[i]);
        }
        return filterArray;
    }
    RawArray<Any> map(const CallbackF_Basic<Any> &mapFn)
    {
        auto f = convert_basic<Any>(mapFn);
        Any *mapData = static_cast<Any *>(::operator new(length * sizeof(Any)));
        for (size_t i = 0; i < length; i++)
            ::new (mapData + i) Any(std::move(f(data[i], i, *this)));
        RawArray<Any> mapArray(mapData, length);
        return mapArray;
    }
    RawArray<T> concat(const RawArray<T> &otherArray)
    {
        RawArray<T> concatArray{*this};
        for (size_t i = 0; i < otherArray.length; i++)
            concatArray.push(otherArray.data[i]);
        return concatArray;
    }
    RawArray<T> slice(ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt) const
    {
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length);
        end > length && (end = length);
        if (start >= length || end <= start)
            return RawArray<T>(0);
        RawArray<T> sliceArray(0);
        for (size_t i = 0; i < end - start; i++)
            sliceArray.push(data[start + i]);
        return sliceArray;
    }
    RawArray<T> splice(ptrdiff_t start, std::optional<size_t> _deleteCount = std::nullopt, const std::initializer_list<T> &newElements = {})
    {
        RawArray<T> spliceArray(0);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        if (start >= length)
            return spliceArray;
        size_t deleteCount = (_deleteCount.has_value() ? *_deleteCount : length - start);
        deleteCount > length - start && (deleteCount = length - start);
        deleteCount < 0 && (deleteCount = 0);
        size_t restNum = length - start - deleteCount;
        if (deleteCount > 0)
        {
            T *deleteData = alloc(deleteCount);
            for (size_t i = 0; i < deleteCount; i++)
                construct(deleteData + i, std::move(data[start + i]));
            spliceArray.capacity = deleteCount;
            *const_cast<size_t *>(&spliceArray.length) = deleteCount;
            spliceArray.data = deleteData;
            std::move(data + start + deleteCount, data + length, data + start);
            *const_cast<size_t *>(&length) = length - deleteCount;
        }
        size_t loc = 0;
        for (const auto &ele : newElements)
            insertAt(start + loc++, std::move(ele));
        return spliceArray;
    }
    RawArray<T> &reverse()
    {
        for (size_t i = 0; i < length / 2; i++)
        {
            T c{data[i]};
            data[i] = data[length - 1 - i];
            data[length - 1 - i] = c;
        }
        return *this;
    }
    RawArray<T> &sort(std::optional<const std::function<ptrdiff_t(T, T)>> _compareFn = std::nullopt)
    {
        if (!_compareFn.has_value())
            std::sort(data, data + length, std::less<T>());
        else
        {
            auto &compareFn = *_compareFn;
            std::sort(data, data + length, [&compareFn](T a, T b)
                      { return compareFn(a, b) < 0; });
        }
        return *this;
    }
    RawArray<T> &copyWithin(ptrdiff_t target, ptrdiff_t start, std::optional<ptrdiff_t> _end = std::nullopt)
    {
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (target < 0 && target > -length) && (target += length) || target < 0 && (target = 0);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length);
        end > length && (end = length);
        if (target >= length || start >= length || end <= start || target == start)
            return *this;
        size_t dist = std::min(end - start, end - target);
        T *tmp = alloc(dist);
        for (size_t i = 0; i < dist; i++)
            construct(tmp + i, std::move(data[start + i]));
        for (size_t i = 0; i < dist; i++)
        {
            destroy(data + target + i);
            construct(data + target + i, std::move(tmp[i]));
        }
        return *this;
    }
    RawArray<T> &fill(const T &value, ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt)
    {
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length);
        end > length && (end = length);
        for (size_t i = start; i < end; i++)
        {
            destroy(data + i);
            construct(data + i, value);
        }
        return *this;
    }
    //////////////////////////////////Operator
    T &operator[](size_t index)
    {
        if (index >= length)
        {
            size_t oldLength = length;
            for (size_t i = length; i < index + 1; i++)
                push(T{});
        }
        else if (index < 0)
            index = 0;
        return data[index];
    }
    RawArray<T> &operator=(const RawArray<T> &newArray)
    {
        if (this != &newArray)
        {
            RawArray<T>(newArray.length).swap(*this);
            std::copy(newArray.data, newArray.data + newArray.length, data);
        }
        return *this;
    }
    RawArray<T> &operator=(RawArray<T> &&newArray)
    {
        capacity = std::__exchange(newArray.capacity, 0);
        *const_cast<size_t *>(&length) = std::__exchange(*const_cast<size_t *>(&newArray.length), 0);
        data = std::__exchange(newArray.data, nullptr);
        return *this;
    }
    friend std::ostream &operator<<(std::ostream &os, const RawArray<T> &array)
    {
        T *data = array.data;
        if (array.length > 1)
            os << "\n(" << array.length << ") ";
        os << "[";
        for (size_t i = 0; i < array.length; i++)
        {
            os << data[i] << (i == array.length - 1 ? "" : ", ");
        }
        os << "]\n";
        return os;
    }
    //////////////////////////////////Desstructor
    ~RawArray() noexcept
    {
        clear();
        dealloc(data);
    }
};

#endif // _RAWARRAY_H_