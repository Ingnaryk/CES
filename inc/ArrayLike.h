#ifndef _ARRAYLIKE_H_
#define _ARRAYLIKE_H_

#include <memory>
#include "Any.h"

template <typename T>
class ArrayLike
{
    template <typename U>
    using CallbackF = std::variant<std::function<U()>,
                                   std::function<U(T)>,
                                   std::function<U(T, size_t)>,
                                   std::function<U(T, size_t, ArrayLike<T> &)>>;
    template <typename newT>
    std::function<newT(T, size_t, ArrayLike<T> &)> convert(const CallbackF<newT> &fn)
    {
        std::function<newT(T, size_t, ArrayLike<T> &)> f;
        switch (fn.index())
        {
        case 0:
            f = [&fn](T, size_t, ArrayLike<T> &)
            { return std::get<0>(fn)(); };
            break;
        case 1:
            f = [&fn](T value, size_t, ArrayLike<T> &)
            { return std::get<1>(fn)(value); };
            break;
        case 2:
            f = [&fn](T value, size_t index, ArrayLike<T> &)
            { return std::get<2>(fn)(value, index); };
            break;
        case 3:
            f = [&fn](T value, size_t index, ArrayLike<T> &thisArray)
            { return std::get<3>(fn)(value, index, thisArray); };
            break;
        }
        return f;
    }

private:
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
    void swap(ArrayLike<T> &otherArray) noexcept
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
                construct(newData + i, std::move_if_noexcept(data[i]));
            capacity = newCapacity;
            data = newData;
        }
        for (ptrdiff_t i = length; i > loc; i--)
            construct(data + i, std::move_if_noexcept(data[i - 1]));
        construct(data + loc, element);
        *const_cast<size_t *>(&length) = length + 1;
    }
    size_t capacity{0};
    T *data{nullptr};

public:
    //////////////////////////////////Variadic templates functions' end
    template <typename Arg>
    size_t push(const Arg &element)
    {
        insertAt(length, std::move_if_noexcept(element));
        return length;
    }
    template <typename Arg>
    size_t unshift(const Arg &element)
    {
        insertAt(0, std::move_if_noexcept(element));
        return length;
    }
    //////////////////////////////////ES Properties
    const size_t length{0};
    //////////////////////////////////Constructor
    ArrayLike() noexcept = default;
    explicit ArrayLike(size_t size) : capacity{size}, data{alloc(capacity)}
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (; _length < size; _length++)
            construct(data + _length); // call T's default constructor
    }
    ArrayLike(T *otherData, size_t size) : capacity{size}, data{alloc(capacity)}
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (; _length < size; _length++)
            construct(data + _length, std::move_if_noexcept(otherData[_length]));
    }
    ArrayLike(const std::initializer_list<T> &elements) : capacity{elements.size()}, data{alloc(capacity)}
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (const auto &ele : elements)
            construct(data + _length++, std::move_if_noexcept(ele));
    }
    ArrayLike(const ArrayLike<T> &otherArray) : capacity{otherArray.length}, data{alloc(capacity)}
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (; _length < otherArray.length; _length++)
            construct(data + _length, otherArray.data[_length]); // call T's copy constructor
    }
    ArrayLike(ArrayLike<T> &&rvArray) noexcept
    {
        swap(rvArray);
    }
    //////////////////////////////////ES Functions
    template <typename Arg, typename... RestArgs>
    size_t push(const Arg &element, const RestArgs &...elements)
    {
        push(std::move_if_noexcept(element));
        push(std::move_if_noexcept(elements)...);
        return length;
    }
    template <typename Arg, typename... RestArgs>
    size_t unshift(const Arg &element, const RestArgs &...elements)
    {
        unshift(std::move_if_noexcept(elements)...);
        unshift(std::move_if_noexcept(element));
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
                construct(newData + i, std::move_if_noexcept(data[i]));
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
                construct(newData + i, std::move_if_noexcept(data[i]));
            capacity = newCapacity;
            data = newData;
        }
        destroy(data + 0);
        for (size_t i = 0; i < length - 1; i++)
            construct(data + i, std::move_if_noexcept(data[i + 1]));
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
    std::string join(const std::string &separator = ",") const
    {
        std::stringstream ss;
        for (size_t i = 0; i < length; i++)
        {
            ss << data[i] << (i == length - 1 ? "" : separator);
        }
        return ss.str();
    }
    void forEach(const CallbackF<void> &callbackFn)
    {
        auto f = convert<void>(callbackFn);
        for (size_t i = 0; i < length; i++)
            f(data[i], i, *this);
    }
    bool some(const CallbackF<bool> &predicate)
    {
        auto f = convert<bool>(predicate);
        for (size_t i = 0; i < length; i++)
        {
            if (f(data[i], i, *this))
                return true;
        }
        return false;
    }
    bool every(const CallbackF<bool> &predicate)
    {
        auto f = convert<bool>(predicate);
        for (size_t i = 0; i < length; i++)
        {
            if (!f(data[i], i, *this))
                return false;
        }
        return true;
    }
    Any find(const CallbackF<bool> &predicate)
    {
        auto f = convert<bool>(predicate);
        for (size_t i = 0; i < length; i++)
        {
            if (f(data[i], i, *this))
                return data[i];
        }
        return undefined;
    }
    Any findLast(const CallbackF<bool> &predicate)
    {
        auto f = convert<bool>(predicate);
        for (size_t i = length - 1; i >= 0; i--)
        {
            if (f(data[i], i, *this))
                return data[i];
        }
        return undefined;
    }
    ptrdiff_t findIndex(const CallbackF<bool> &predicate)
    {
        auto f = convert<bool>(predicate);
        for (size_t i = 0; i < length; i++)
        {
            if (f(data[i], i, *this))
                return i;
        }
        return -1;
    }
    ptrdiff_t findLastIndex(const CallbackF<bool> &predicate)
    {
        auto f = convert<bool>(predicate);
        for (size_t i = length - 1; i >= 0; i--)
        {
            if (f(data[i], i, *this))
                return i;
        }
        return -1;
    }
    Any reduce(const std::function<Any(T, T, size_t, ArrayLike<T> *)> &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
    {
        if (length <= 0)
            return undefined;
        ptrdiff_t crtIndex = 0;
        T accumulator;
        if (_initialValue)
            accumulator = _initialValue.value();
        else
        {
            accumulator = data[crtIndex];
            crtIndex++;
        }
        for (ptrdiff_t i = crtIndex; i < length; i++)
            accumulator = callbackFn(accumulator, data[i], i, this);
        return accumulator;
    }
    Any reduceRight(const std::function<Any(T, T, size_t, ArrayLike<T> *)> &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
    {
        if (length <= 0)
            return undefined;
        ptrdiff_t crtIndex = length - 1;
        T accumulator;
        if (_initialValue)
            accumulator = _initialValue.value();
        else
        {
            accumulator = data[crtIndex];
            crtIndex--;
        }
        for (ptrdiff_t i = crtIndex; i >= 0; i--)
            accumulator = callbackFn(accumulator, data[i], i, this);
        return accumulator;
    }
    ArrayLike<T> filter(const CallbackF<bool> &predicate)
    {
        auto f = convert<bool>(predicate);
        ArrayLike<T> filterArray(0);
        size_t filterLength = 0;
        for (size_t i = 0; i < length; i++)
        {
            if (f(data[i], i, *this))
                filterArray.push(data[i]);
        }
        return filterArray;
    }
    ArrayLike<Any> map(const CallbackF<Any> &mapFn)
    {
        auto f = convert<Any>(mapFn);
        Any *mapData = static_cast<Any *>(::operator new(length * sizeof(Any)));
        for (size_t i = 0; i < length; i++)
            ::new (mapData + i) Any(std::move(f(data[i], i, *this)));
        ArrayLike<Any> mapArray(mapData, length);
        return mapArray;
    }
    ArrayLike<T> concat(const ArrayLike<T> &otherArray)
    {
        ArrayLike<T> concatArray{*this};
        for (size_t i = 0; i < otherArray.length; i++)
            concatArray.push(otherArray.data[i]);
        return concatArray;
    }
    ArrayLike<T> slice(ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt) const
    {
        ptrdiff_t end;
        _end && (end = _end.value()) || (end = length - 1);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length);
        end >= length && (end = length - 1);
        if (start >= length || end <= start)
            return ArrayLike<T>(0);
        ArrayLike<T> sliceArray(0);
        for (size_t i = 0; i < end - start; i++)
            sliceArray.push(data[start + i]);
        return sliceArray;
    }
    ArrayLike<T> splice(ptrdiff_t start, std::optional<size_t> _deleteCount = std::nullopt, const std::initializer_list<T> &newElements = {})
    {
        size_t deleteCount;
        size_t restNum;
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        if (start >= length)
            return ArrayLike<T>(0);
        _deleteCount && (deleteCount = _deleteCount.value()) || (deleteCount = length - start);
        deleteCount > length - start && (deleteCount = length - start);
        deleteCount < 0 && (deleteCount = 0);
        restNum = length - start - deleteCount;
        ArrayLike<T> deleteArray(0);
        if (deleteCount > 0)
        {
            T *deleteData = alloc(deleteCount);
            for (size_t i = 0; i < deleteCount; i++)
                construct(deleteData + i, std::move_if_noexcept(data[start + i]));
            deleteArray.capacity = deleteCount;
            *const_cast<size_t *>(&deleteArray.length) = deleteCount;
            deleteArray.data = deleteData;
            std::move(data + start + deleteCount, data + length, data + start);
            *const_cast<size_t *>(&length) = length - deleteCount;
        }
        size_t loc = 0;
        for (const auto &ele : newElements)
            insertAt(start + loc++, std::move_if_noexcept(ele));
        return deleteArray;
    }
    ArrayLike<T> &reverse()
    {
        for (size_t i = 0; i < length / 2; i++)
        {
            T c{data[i]};
            data[i] = data[length - 1 - i];
            data[length - 1 - i] = c;
        }
        return *this;
    }
    ArrayLike<T> &sort(std::optional<const std::function<ptrdiff_t(T, T)>> _compareFn = std::nullopt)
    {
        if (!_compareFn)
            std::sort(data, data + length, std::less<T>());
        else
        {
            auto &compareFn = _compareFn.value();
            std::sort(data, data + length, [&compareFn](T a, T b)
                      { return compareFn(a, b) < 0; });
        }
        return *this;
    }
    ArrayLike<T> &copyWithin(ptrdiff_t target, ptrdiff_t start, std::optional<ptrdiff_t> _end = std::nullopt)
    {
        ptrdiff_t end;
        _end && (end = _end.value()) || (end = length);
        (target < 0 && target > -length) && (target += length) || target < 0 && (target = 0);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length);
        end > length && (end = length);
        if (target >= length || start >= length || end <= start || target == start)
            return *this;
        size_t dist = std::min(end - start, end - target);
        T *tmp = alloc(dist);
        for (size_t i = 0; i < dist; i++)
            construct(tmp + i, std::move_if_noexcept(data[start + i]));
        for (size_t i = 0; i < dist; i++)
        {
            destroy(data + target + i);
            construct(data + target + i, std::move_if_noexcept(tmp[i]));
        }
        return *this;
    }
    ArrayLike<T> &fill(const T &value, ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt)
    {
        ptrdiff_t end;
        _end && (end = _end.value()) || (end = length);
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
    Any &operator[](size_t index)
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
    ArrayLike<T> &operator=(const ArrayLike<T> &newArray)
    {
        if (this != &newArray)
        {
            ArrayLike<T>(newArray.length).swap(*this);
            std::copy(newArray.data, newArray.data + newArray.length, data);
        }
        return *this;
    }
    ArrayLike<T> &operator=(ArrayLike<T> &&newArray)
    {
        capacity = std::__exchange(newArray.capacity, 0);
        *const_cast<size_t *>(&length) = std::__exchange(*const_cast<size_t *>(&newArray.length), 0);
        data = std::__exchange(newArray.data, nullptr);
        return *this;
    }
    friend std::ostream &operator<<(std::ostream &os, const ArrayLike<T> &array)
    {
        T *data = array.data;
        os << "\n(" << array.length << ") [";
        for (size_t i = 0; i < array.length; i++)
        {
            os << data[i] << (i == array.length - 1 ? "" : ", ");
        }
        os << "]\n";
        return os;
    }
    //////////////////////////////////Desstructor
    ~ArrayLike() noexcept
    {
        clear();
        dealloc(data);
    }
};

#endif //_ARRAYLIKE_H_