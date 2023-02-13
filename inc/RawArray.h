#ifndef _RAWARRAY_H_
#define _RAWARRAY_H_

#include <memory>

#include "Any.h"

template <typename T>
class RawArray
{
private:
    //////////////////////////////////Internal usage
    void insertAt(size_t loc, const T &element)
    {
        if (length == capacity)
        {
            size_t newCapacity = (capacity ? capacity * 2 : 1);
            T *newData = (T *)(::operator new(newCapacity * sizeof(T)));
            for (size_t i = 0; i < length; i++)
                ::new (newData + i) T(std::move(data[i]));
            capacity = newCapacity;
            ::operator delete(data);
            data = newData;
        }

        for (size_t i = length; i > loc; i--)
            ::new (data + i) T(std::move(data[i - 1]));
        ::new (data + loc) T(element);

        *const_cast<size_t *>(&length) = length + 1;
    }
    //////////////////////////////////Wrapped data
    size_t capacity{0};
    T *data{nullptr};

public:
    //////////////////////////////////Variadic templates functions' end
    template <class First>
        requires(std::is_same_v<First, T> || std::is_same_v<T, Any>)
    size_t push(const First &element)
    {
        insertAt(length, element);
        return length;
    }
    template <class First>
        requires(std::is_same_v<First, T> || std::is_same_v<T, Any>)
    size_t unshift(const First &element)
    {
        insertAt(0, element);
        return length;
    }
    //////////////////////////////////ES Property
    const size_t length{0};
    //////////////////////////////////Constructor
    explicit RawArray(size_t size) : capacity{size}, data{(T *)(::operator new(capacity * sizeof(T)))}
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (; _length < size; _length++)
            ::new (data + _length) T();
    }
    RawArray(T *otherData, size_t size) : capacity{size}, data{(T *)(::operator new(capacity * sizeof(T)))}
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (; _length < size; _length++)
            ::new (data + _length) T(otherData[_length]);
    }
    RawArray(T *otherData, size_t size, size_t capacity) : length{size}, capacity{capacity}, data{otherData} {}
    RawArray(const std::initializer_list<T> &elements) : capacity{elements.size()}, data{(T *)(::operator new(capacity * sizeof(T)))}
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (const T &ele : elements)
            ::new (data + _length++) T(ele);
    }
    RawArray(const RawArray<T> &otherArray) : capacity{otherArray.capacity}, data{(T *)(::operator new(capacity * sizeof(T)))}
    {
        size_t &_length = *const_cast<size_t *>(&length);
        for (; _length < otherArray.length; _length++)
            ::new (data + _length) T(*(otherArray.data + _length));
    }
    RawArray(RawArray<T> &&rvArray) : capacity{rvArray.capacity}, length{rvArray.length}, data{rvArray.data}
    {
        rvArray.data = nullptr;
    }
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
    Any pop()
    {
        if (length <= 0)
            return undefined;
        T last = std::move(data[length - 1]);
        *const_cast<size_t *>(&length) = length - 1;
        return last;
    }
    Any shift()
    {
        if (length <= 0)
            return undefined;
        T first = std::move(data[0]);
        if (length - 1 > 0)
        {
            for (size_t i = 0; i < length - 1; i++)
                ::new (data + i) T(std::move(data[i + 1]));
        }
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
    void forEach(const std::function<void(T, size_t, std::reference_wrapper<RawArray<T>>)> &callbackFn)
    {
        for (size_t i = 0; i < length; i++)
            callbackFn(data[i], i, std::ref(*this));
    }
    bool some(const std::function<bool(T, size_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        for (size_t i = 0; i < length; i++)
        {
            if (predicate(data[i], i, std::ref(*this)))
                return true;
        }
        return false;
    }
    bool every(const std::function<bool(T, size_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        for (size_t i = 0; i < length; i++)
        {
            if (!predicate(data[i], i, std::ref(*this)))
                return false;
        }
        return true;
    }
    Any find(const std::function<bool(T, size_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        for (size_t i = 0; i < length; i++)
        {
            if (predicate(data[i], i, std::ref(*this)))
                return data[i];
        }
        return undefined;
    }
    Any findLast(const std::function<bool(T, size_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        for (size_t i = length - 1; i >= 0; i--)
        {
            if (predicate(data[i], i, std::ref(*this)))
                return data[i];
        }
        return undefined;
    }
    ptrdiff_t findIndex(const std::function<bool(T, size_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        for (size_t i = 0; i < length; i++)
        {
            if (predicate(data[i], i, std::ref(*this)))
                return i;
        }
        return -1;
    }
    ptrdiff_t findLastIndex(const std::function<bool(T, size_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        for (size_t i = length - 1; i >= 0; i--)
        {
            if (predicate(data[i], i, std::ref(*this)))
                return i;
        }
        return -1;
    }
    T reduce(const std::function<T(T, T, size_t, std::reference_wrapper<RawArray<T>>)> &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
    {
        if (length <= 0)
            return T{};
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
            accumulator = callbackFn(accumulator, data[i], i, std::ref(*this));
        return accumulator;
    }
    T reduceRight(const std::function<T(T, T, size_t, std::reference_wrapper<RawArray<T>>)> &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
    {
        if (length <= 0)
            return T{};
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
            accumulator = callbackFn(accumulator, data[i], i, std::ref(*this));
        return accumulator;
    }
    RawArray<T> filter(const std::function<bool(T, size_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        RawArray<T> filterArray(0);
        size_t filterLength = 0;
        for (size_t i = 0; i < length; i++)
        {
            if (predicate(data[i], i, std::ref(*this)))
                filterArray.push(data[i]);
        }
        return filterArray;
    }
    RawArray<Any> map(const std::function<Any(T, size_t, std::reference_wrapper<RawArray<T>>)> &mapFn)
    {
        Any *mapData = static_cast<Any *>(::operator new(length * sizeof(Any)));
        for (size_t i = 0; i < length; i++)
            ::new (mapData + i) Any(mapFn(data[i], i, std::ref(*this)));
        RawArray<Any> mapArray(mapData, length, length);
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
        RawArray<T> sliceArray(0);
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length);
        end > length && (end = length);
        if (start >= length || end <= start)
            return sliceArray;
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
            ::operator delete(spliceArray.data);
            T *deleteData = (T *)(::operator new(deleteCount * sizeof(T)));
            for (size_t i = 0; i < deleteCount; i++)
                ::new (deleteData + i) T(std::move(data[start + i]));
            spliceArray.capacity = deleteCount;
            *const_cast<size_t *>(&spliceArray.length) = deleteCount;
            spliceArray.data = deleteData;
            for (size_t i = start; i < restNum; i++)
                ::new (data + i) T(std::move(data[i + deleteCount]));
            *const_cast<size_t *>(&length) = length - deleteCount;
        }
        size_t loc = 0;
        for (const T &ele : newElements)
            insertAt(start + loc++, std::move(ele));
        return spliceArray;
    }
    RawArray<T> &reverse()
    {
        std::reverse(data, data + length);
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
        T *tmp = (T *)(::operator new(dist * sizeof(T)));
        for (size_t i = 0; i < dist; i++)
            ::new (tmp + i) T(std::move(data[start + i]));
        for (size_t i = 0; i < dist; i++)
        {
            (data + target + i)->~T();
            ::new (data + target + i) T(std::move(tmp[i]));
        }
        ::operator delete(tmp);
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
            (data + i)->~T();
            ::new (data + i) T(value);
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
    RawArray<T> &operator=(const RawArray<T> &otherArray)
    {
        if (this == &otherArray)
            return *this;
        if (capacity < otherArray.length)
        {
            ~RawArray();
            capacity = otherArray.length;
            ::operator new(capacity * sizeof(T));
        }
        else
        {
            for (size_t i = 0; i < length; i++)
                (data + i)->~T();
        }
        *const_cast<size_t *>(&length) = otherArray.length;
        for (size_t i = 0; i < length; i++)
            ::new (data + i) T(otherArray.data[i]);
        return *this;
    }
    RawArray<T> &operator=(RawArray<T> &&rvArray)
    {
        capacity = rvArray.capacity;
        *const_cast<size_t *>(&length) = rvArray.length;
        std::swap(data, rvArray.data);
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
        if (data)
        {
            for (size_t i = 0; i < length; i++)
                (data + i)->~T();
            ::operator delete(data);
        }
    }
};

#endif // _RAWARRAY_H_