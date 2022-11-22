#include "../inc/Array.h"
using namespace std;

//////////////////////////////////Constructor
Array::Array(size_t size) : data(size), length{size} {}
Array::Array(const initializer_list<Any> &elements) : length{elements.size()}
{
    data.insert(data.end(), elements);
}
Array::Array(const Array &otherArray) : data{otherArray.data}, length{otherArray.length} {}
Array::Array(Array &&rvArray) noexcept
{
    swap(*const_cast<size_t *>(&length), *const_cast<size_t *>(&rvArray.length));
    data.swap(rvArray.data);
}
//////////////////////////////////ES Functions
Any Array::pop()
{
    Any back = data.back();
    data.pop_back();
    *const_cast<size_t *>(&length) = length - 1;
    return back;
}
Any Array::shift()
{
    Any front = data.front();
    data.pop_front();
    *const_cast<size_t *>(&length) = length - 1;
    return front;
}
Any Array::at(ptrdiff_t index) const
{
    index < 0 && (index += length);
    if (index < 0 || index >= length)
        return undefined;
    auto itor = data.cbegin();
    advance(itor, index);
    return *itor;
}
ptrdiff_t Array::indexOf(const Any &value, ptrdiff_t fromIndex = 0) const
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
ptrdiff_t Array::lastIndexOf(const Any &value, ptrdiff_t fromIndex = -1) const
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
bool Array::includes(const Any &value) const
{
    return ::find(data.cbegin(), data.cend(), value) != data.cend();
}
string Array::join(const string &seperator) const
{
    stringstream ss;
    size_t crtIndex = 0;
    for (const Any &val : data)
    {
        ss << val << (crtIndex++ == length - 1 ? "" : seperator);
    }
    return ss.str();
}
void Array::forEach(const CallbackF<void> &callbackFn)
{
    auto f = convert<void>(callbackFn);
    size_t crtIndex = 0;
    for (Any val : data)
        f(val, crtIndex++, *this);
}
bool Array::some(const CallbackF<bool> &predicate)
{
    auto f = convert<bool>(predicate);
    size_t crtIndex = 0;
    for (Any val : data)
    {
        if (f(val, crtIndex++, *this))
            return true;
    }
    return false;
}
bool Array::every(const CallbackF<bool> &predicate)
{
    auto f = convert<bool>(predicate);
    size_t crtIndex = 0;
    for (Any val : data)
    {
        if (!f(val, crtIndex++, *this))
            return false;
    }
    return true;
}
Any Array::find(const CallbackF<bool> &predicate)
{
    auto f = convert<bool>(predicate);
    size_t crtIndex = 0;
    for (auto itor = data.cbegin(); itor != data.cend(); itor++)
    {
        if (f(*itor, crtIndex++, *this))
            return *itor;
    }
    return undefined;
}
Any Array::findLast(const CallbackF<bool> &predicate)
{
    auto f = convert<bool>(predicate);
    size_t crtIndex = length - 1;
    for (auto itor = data.crbegin(); itor != data.crend(); itor++)
    {
        if (f(*itor, crtIndex--, *this))
            return *itor;
    }
    return undefined;
}
ptrdiff_t Array::findIndex(const CallbackF<bool> &predicate)
{
    auto f = convert<bool>(predicate);
    size_t crtIndex = 0;
    for (auto itor = data.cbegin(); itor != data.cend(); itor++)
    {
        if (f(*itor, crtIndex++, *this))
            return --crtIndex;
    }
    return -1;
}
ptrdiff_t Array::findLastIndex(const CallbackF<bool> &predicate)
{
    auto f = convert<bool>(predicate);
    size_t crtIndex = length - 1;
    for (auto itor = data.crbegin(); itor != data.crend(); itor++)
    {
        if (f(*itor, crtIndex--, *this))
            return ++crtIndex;
    }
    return -1;
}
Any Array::reduce(const function<Any(Any, Any, size_t, Array *)> &callbackFn, const optional<Any> &_initialValue)
{
    if (length == 0)
        return undefined;
    auto itor = data.cbegin();
    size_t crtIndex = 0;
    Any accumulator;
    if (_initialValue)
        accumulator = _initialValue.value();
    else
    {
        accumulator = data.front();
        itor++;
        crtIndex++;
    }
    for (; itor != data.cend(); itor++)
    {
        accumulator = callbackFn(accumulator, *itor, crtIndex++, this);
    }
    return accumulator;
}
Any Array::reduceRight(const function<Any(Any, Any, size_t, Array *)> &callbackFn, const optional<Any> &_initialValue)
{
    if (length == 0)
        return undefined;
    auto itor = data.crbegin();
    size_t crtIndex = length - 1;
    Any accumulator;
    if (_initialValue)
        accumulator = _initialValue.value();
    else
    {
        accumulator = data.back();
        itor++;
        crtIndex--;
    }
    for (; itor != data.crend(); itor++)
    {
        accumulator = callbackFn(accumulator, *itor, crtIndex--, this);
    }
    return accumulator;
}
Array Array::filter(const CallbackF<bool> &predicate)
{
    auto f = convert<bool>(predicate);
    Array filterArray(0);
    size_t crtIndex = 0;
    for (Any val : data)
    {
        if (f(val, crtIndex++, *this))
            filterArray.push(val);
    }
    return filterArray;
}
Array Array::map(const CallbackF<Any> &mapFn)
{
    auto f = convert<Any>(mapFn);
    Array mapArray(0);
    size_t crtIndex = 0;
    for (Any val : data)
        mapArray.push(std::move(f(val, crtIndex++, *this)));
    return mapArray;
}
Array Array::concat(const Array &otherArray) const
{
    Array concatArray{*this};
    for (const Any &ele : otherArray.data)
        concatArray.push(ele);
    return concatArray;
}
Array Array::slice(ptrdiff_t start, optional<ptrdiff_t> _end) const
{
    ptrdiff_t end;
    _end && (end = _end.value()) || (end = length - 1);
    (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
    (end < 0 && end > -length) && (end += length);
    end >= length && (end = length - 1);
    if (start >= length || end <= start)
        return Array(0);
    Array sliceArray(end - start);
    auto sitor = data.cbegin();
    auto eitor = data.cbegin();
    advance(sitor, start);
    advance(eitor, end);
    copy(sitor, eitor, sliceArray.data.begin());
    return sliceArray;
}
Array Array::splice(ptrdiff_t start, std::optional<size_t> _deleteCount, const std::initializer_list<Any> &newElements)
{
    Array spliceArray(0);
    size_t deleteCount;
    (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
    if (start >= length)
        return spliceArray;
    _deleteCount && (deleteCount = _deleteCount.value()) || (deleteCount = length - start);
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
Array &Array::reverse()
{
    data.reverse();
    return *this;
}
Array &Array::sort(const optional<function<ptrdiff_t(Any, Any)>> &_compareFn)
{
    if (!_compareFn)
        data.sort();
    else
    {
        auto &compareFn = _compareFn.value();
        data.sort([&compareFn](Any a, Any b)
                  { return compareFn(a, b) < 0; });
    }
    return *this;
}
Array &Array::copyWithin(ptrdiff_t target, ptrdiff_t start, optional<ptrdiff_t> _end)
{
    ptrdiff_t end;
    _end && (end = _end.value()) || (end = length);
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
    copy_n(sitor, min(des, det), titor);
    return *this;
}
Array &Array::fill(const Any &value, ptrdiff_t start, optional<ptrdiff_t> _end)
{
    ptrdiff_t end;
    _end && (end = _end.value()) || (end = length);
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
Any &Array::operator[](size_t index)
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
Array &Array::operator=(const Array &newArray)
{
    if (this != &newArray)
    {
        list<Any>(newArray.data).swap(data);
        *const_cast<size_t *>(&length) = newArray.length;
    }
    return *this;
}
Array &Array::operator=(Array &&newArray)
{
    *const_cast<size_t *>(&length) = std::__exchange(*const_cast<size_t *>(&newArray.length), 0);
    data = std::__exchange(newArray.data, list<Any>{});
    return *this;
}
ostream &operator<<(ostream &os, const Array &array)
{
    os << "\n(" << array.length << ") [";
    size_t crtIndex = 0;
    for (const Any &val : array.data)
    {
        os << val << (crtIndex++ == array.length - 1 ? "" : ", ");
    }
    os << "]\n";
    return os;
}