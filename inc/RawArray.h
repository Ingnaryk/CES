#ifndef _RAWARRAY_H_
#define _RAWARRAY_H_

#include "any.h"

template <typename T>
class RawArray
{
private:
    //////////////////////////////////Internal usage
    void _insertAt(ptrdiff_t loc, const std::initializer_list<T> &elements, bool directly = false)
    {
        if (!directly && length + elements.size() > capacity)
        {
            ptrdiff_t newCapacity = std::max(length, (ptrdiff_t)elements.size()) * 2;
            T *newData = (T *)(::operator new(newCapacity * sizeof(T)));
            if constexpr (std::is_trivially_copyable_v<T>)
            {
                memmove(newData, data, loc * sizeof(T));
                if (loc < length) // give the space for insertion
                    memmove(newData + loc + elements.size(), data + loc, (length - loc) * sizeof(T));
            }
            else
            {
                for (size_t i = 0; i < loc; i++)
                    ::new (newData + i) T{std::move(data[i])};
                // if (loc < length) the for loop will handle the condition
                for (size_t i = 0; i < length - loc; i++)
                    ::new (newData + loc + elements.size() + i) T{std::move(data[loc + i])};
            }
            capacity = newCapacity;
            ::operator delete(data);
            data = newData;
            directly = true;
        }

        if constexpr (std::is_trivially_copyable_v<T>)
        {
            if (!directly) // move-right
                memmove(data + loc + elements.size(), data + loc, elements.size() * sizeof(T));
            memmove(data + loc, elements.begin(), elements.size() * sizeof(T));
        }
        else
        {
            if (!directly) // move-right
                for (ptrdiff_t i = length - 1; i >= loc; i--)
                    ::new (data + i + elements.size()) T{std::move(data[i])};
            for (const T &ele : elements)
                ::new (data + loc++) T{ele};
        }

        const_cast<ptrdiff_t &>(length) += elements.size();
    }
    //////////////////////////////////Variadic templates functions' end
    ptrdiff_t _push(std::initializer_list<T> elements)
    {
        _insertAt(length, elements);
        return length;
    }
    ptrdiff_t _unshift(std::initializer_list<T> elements)
    {
        _insertAt(0, elements);
        return length;
    }
    //////////////////////////////////Wrapped data
    ptrdiff_t capacity{0};
    T *data{nullptr};

public:
    //////////////////////////////////ES Property
    const ptrdiff_t length{0};
    //////////////////////////////////Constructor
    explicit RawArray(ptrdiff_t size) : capacity{size}, length{size}, data{(T *)(::operator new(capacity * sizeof(T)))} {}
    RawArray(const std::initializer_list<T> &elements) : capacity{(ptrdiff_t)elements.size()}, data{(T *)(::operator new(capacity * sizeof(T)))}
    {
        if constexpr (std::is_trivially_copyable_v<T>)
        {
            const_cast<ptrdiff_t &>(length) = elements.size();
            memmove(data, elements.begin(), length * sizeof(T));
        }
        else
        {
            ptrdiff_t &_length = const_cast<ptrdiff_t &>(length);
            for (const T &ele : elements)
                ::new (data + _length++) T{ele};
        }
    }
    RawArray(const RawArray<T> &otherArray) : capacity{otherArray.capacity}, data{(T *)(::operator new(capacity * sizeof(T)))}
    {
        if constexpr (std::is_trivially_copyable_v<T>)
        {
            const_cast<ptrdiff_t &>(length) = otherArray.length;
            memmove(data, otherArray.data, length * sizeof(T));
        }
        else
        {
            ptrdiff_t &_length = const_cast<ptrdiff_t &>(length);
            for (; _length < otherArray.length; _length++)
                ::new (data + _length) T{otherArray.data[_length]};
        }
    }
    RawArray(RawArray<T> &&rvArray) : capacity{rvArray.capacity}, length{rvArray.length}, data{rvArray.data}
    {
        rvArray.data = nullptr;
    }
    //////////////////////////////////ES Method
    template <typename... Rest>
    ptrdiff_t push(const T &element, const Rest &...elements)
    {
        _push(std::initializer_list<T>{element, elements...});
        return length;
    }
    template <typename... Rest>
    ptrdiff_t unshift(const T &element, const Rest &...elements)
    {
        _unshift(std::initializer_list<T>{element, elements...});
        return length;
    }
    any pop()
    {
        if (length <= 0)
            return undefined;
        T last = std::move(data[length - 1]);
        const_cast<ptrdiff_t &>(length)--;
        return last;
    }
    any shift()
    {
        if (length <= 0)
            return undefined;
        T first = std::move(data[0]);
        if (length - 1 > 0)
        {
            if constexpr (std::is_trivially_copyable_v<T>)
                memmove(data, data + 1, (length - 1) * sizeof(T));
            else
                std::move(data + 1, data + length, data);
        }
        const_cast<ptrdiff_t &>(length)--;
        return first;
    }
    any at(ptrdiff_t index) const
    {
        index < 0 && (index += length);
        if (index < 0 || index >= length)
            return undefined;
        return data[index];
    }
    ptrdiff_t indexOf(const T &value, ptrdiff_t fromIndex = 0) const
    {
        (fromIndex < 0 && fromIndex > -length) && (fromIndex += length) || fromIndex < 0 && (fromIndex = 0);
        for (ptrdiff_t i = fromIndex; i < length; i++)
        {
            if (data[i] == value)
                return i;
        }
        return -1;
    }
    ptrdiff_t lastIndexOf(const T &value, ptrdiff_t fromIndex = -1) const
    {
        (fromIndex < 0 && fromIndex > -length) && (fromIndex += length) || fromIndex >= length && (fromIndex = length - 1);
        for (ptrdiff_t i = fromIndex; i >= 0; i--)
        {
            if (data[i] == value)
                return i;
        }
        return -1;
    }
    bool includes(const T &value) const
    {
        for (ptrdiff_t i = 0; i < length; i++)
        {
            if (data[i] == value)
                return true;
        }
        return false;
    }
    std::string join(const char *separator = ",") const
    {
        std::stringstream ss;
        for (ptrdiff_t i = 0; i < length; i++)
        {
            ss << data[i] << (i == length - 1 ? "" : separator);
        }
        return ss.str();
    }
    void forEach(const std::function<void(T, ptrdiff_t, std::reference_wrapper<RawArray<T>>)> &callbackFn)
    {
        for (ptrdiff_t i = 0; i < length; i++)
            callbackFn(data[i], i, std::ref(*this));
    }
    bool some(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        for (ptrdiff_t i = 0; i < length; i++)
        {
            if (predicate(data[i], i, std::ref(*this)))
                return true;
        }
        return false;
    }
    bool every(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        for (ptrdiff_t i = 0; i < length; i++)
        {
            if (!predicate(data[i], i, std::ref(*this)))
                return false;
        }
        return true;
    }
    any find(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        for (ptrdiff_t i = 0; i < length; i++)
        {
            if (predicate(data[i], i, std::ref(*this)))
                return data[i];
        }
        return undefined;
    }
    any findLast(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        for (ptrdiff_t i = length - 1; i >= 0; i--)
        {
            if (predicate(data[i], i, std::ref(*this)))
                return data[i];
        }
        return undefined;
    }
    ptrdiff_t findIndex(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        for (ptrdiff_t i = 0; i < length; i++)
        {
            if (predicate(data[i], i, std::ref(*this)))
                return i;
        }
        return -1;
    }
    ptrdiff_t findLastIndex(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        for (ptrdiff_t i = length - 1; i >= 0; i--)
        {
            if (predicate(data[i], i, std::ref(*this)))
                return i;
        }
        return -1;
    }
    T reduce(const std::function<T(T, T, ptrdiff_t, std::reference_wrapper<RawArray<T>>)> &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
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
    T reduceRight(const std::function<T(T, T, ptrdiff_t, std::reference_wrapper<RawArray<T>>)> &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
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
    RawArray<T> filter(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawArray<T>>)> &predicate)
    {
        RawArray<T> filterArray(0);
        for (ptrdiff_t i = 0; i < length; i++)
        {
            if (predicate(data[i], i, std::ref(*this)))
                filterArray.push(data[i]);
        }
        return filterArray;
    }
    template <typename U>
    RawArray<U> map(const std::function<U(T, ptrdiff_t, std::reference_wrapper<RawArray<T>>)> &mapFn)
    {
        RawArray<U> mapArray(0);
        for (ptrdiff_t i = 0; i < length; i++)
            mapArray.push(mapFn(data[i], i, std::ref(*this)));
        return mapArray;
    }
    RawArray<T> concat(const RawArray<T> &otherArray) const
    {
        RawArray<T> concatArray{*this};
        for (ptrdiff_t i = 0; i < otherArray.length; i++)
            concatArray.push(otherArray.data[i]);
        return concatArray;
    }
    RawArray<T> slice(ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt) const
    {
        RawArray<T> sliceArray(0);
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length) || end > length && (end = length);
        if (start >= length || end <= start)
            return sliceArray;
        for (ptrdiff_t i = 0; i < end - start; i++)
            sliceArray.push(data[start + i]);
        return sliceArray;
    }
    RawArray<T> splice(ptrdiff_t start, std::optional<ptrdiff_t> _deleteCount = std::nullopt, const std::initializer_list<T> &newElements = {})
    {
        RawArray<T> spliceArray(0);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        if (start >= length)
            return spliceArray;
        ptrdiff_t deleteCount = (_deleteCount.has_value() ? *_deleteCount : length - start);
        deleteCount > length - start && (deleteCount = length - start) || deleteCount < 0 && (deleteCount = 0);
        ptrdiff_t restNum = length - start - deleteCount;
        bool insertDirectly = true;
        if (deleteCount > 0)
        {
            ::operator delete(spliceArray.data);
            T *deleteData = (T *)(::operator new(deleteCount * sizeof(T)));
            // move the to-be-delete-data to the 'deleteData'
            if constexpr (std::is_trivially_copyable_v<T>)
                memmove(deleteData, data + start, deleteCount * sizeof(T));
            else
                for (ptrdiff_t i = 0; i < deleteCount; i++)
                    ::new (deleteData + i) T{std::move(data[start + i])};
            spliceArray.capacity = deleteCount;
            *const_cast<ptrdiff_t *>(&spliceArray.length) = deleteCount;
            spliceArray.data = deleteData;
            // move the rest data left to fill deleted data
            if constexpr (std::is_trivially_copyable_v<T>)
                memmove(data + start + newElements.size(), data + deleteCount, restNum * sizeof(T));
            else
            {
                if (newElements.size() > deleteCount) // should have move-right, but for the reason of potential resizing, just move-left to connect
                {
                    for (ptrdiff_t i = start; i < start + restNum; i++) // act like newElements.size() == 0
                        ::new (data + i) T{std::move(data[i + deleteCount])};
                    insertDirectly = false;
                }
                else if (newElements.size() < deleteCount) // move-left
                    for (ptrdiff_t i = start + newElements.size(); i < start + restNum; i++)
                        ::new (data + i) T{std::move(data[i + deleteCount - newElements.size()])};
                // if newElements.size() equals to deleteCount, there is no moving
                const_cast<ptrdiff_t &>(length) -= deleteCount;
            }
        }
        _insertAt(start, newElements, insertDirectly);
        return spliceArray;
    }
    RawArray<T> &reverse()
    {
        std::reverse(data, data + length);
        return *this;
    }
    RawArray<T> &sort(const std::optional<std::function<ptrdiff_t(T, T)>> &_compareFn = std::nullopt)
    {
        if (!_compareFn.has_value())
            std::sort(data, data + length, std::less<T>{});
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
        (end < 0 && end > -length) && (end += length) || end > length && (end = length);
        if (target >= length || start >= length || end <= start || target == start)
            return *this;
        ptrdiff_t dist = std::min(end - start, end - target);
        if constexpr (std::is_trivially_copyable_v<T>)
            memmove(data + target, data + start, dist * sizeof(T));
        else
        {
            if (target > start)
                std::copy_backward(data + start, data + start + dist, data + target);
            else
                std::copy(data + start, data + start + dist, data + target);
        }
        return *this;
    }
    RawArray<T> &fill(const T &value, ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt)
    {
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length) || end > length && (end = length);
        for (ptrdiff_t i = start; i < end; i++)
            data[i] = value;
        return *this;
    }
    //////////////////////////////////Operator
    T &operator[](ptrdiff_t index)
    {
        if (index >= length)
        {
            for (ptrdiff_t i = length; i < index + 1; i++)
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
            ptrdiff_t newCapacity = otherArray.capacity;
            T *newData = (T *)(::operator new(newCapacity * sizeof(T)));
            capacity = newCapacity;
            for (ptrdiff_t i = 0; i < length; i++)
                (data + i)->~T();
            ::operator delete(data);
            data = newData;
        }
        else
        {
            for (ptrdiff_t i = 0; i < length; i++)
                (data + i)->~T();
        }
        *const_cast<ptrdiff_t *>(&length) = otherArray.length;
        if constexpr (std::is_trivially_copyable_v<T>)
            memmove(data, otherArray.data, length * sizeof(T));
        else
            std::copy(otherArray.data, otherArray.data + length, data);
        return *this;
    }
    RawArray<T> &operator=(RawArray<T> &&rvArray)
    {
        capacity = rvArray.capacity;
        *const_cast<ptrdiff_t *>(&length) = rvArray.length;
        std::swap(data, rvArray.data);
        return *this;
    }
    friend std::ostream &operator<<(std::ostream &os, const RawArray<T> &array)
    {
        T *data = array.data;
        if (array.length > 1)
            os << "\n(" << array.length << ") ";
        os << "[";
        for (ptrdiff_t i = 0; i < array.length; i++)
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
            for (ptrdiff_t i = 0; i < length; i++)
                (data + i)->~T();
            ::operator delete(data);
        }
    }
};

#endif // _RAWARRAY_H_
