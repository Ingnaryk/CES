#include "../inc/JSArray.h"
using namespace std;

//////////////////////////////////Constructor
JSArray::JSArray(size_t size) : data(size), length{size} {}
JSArray::JSArray(const initializer_list<Any> &elements) : length{elements.size()}
{
    data.insert(data.end(), elements);
}
JSArray::JSArray(const JSArray &otherArray) : data{otherArray.data}, length{otherArray.length} {}
JSArray::JSArray(JSArray &&rvArray) noexcept
{
    swap(*const_cast<size_t *>(&length), *const_cast<size_t *>(&rvArray.length));
    data.swap(rvArray.data);
}
//////////////////////////////////ES Functions
Any JSArray::pop()
{
    Any back = data.back();
    data.pop_back();
    *const_cast<size_t *>(&length) = length - 1;
    return back;
}
Any JSArray::shift()
{
    Any front = data.front();
    data.pop_front();
    *const_cast<size_t *>(&length) = length - 1;
    return front;
}
Any JSArray::at(ptrdiff_t index) const
{
    index < 0 && (index += length);
    if (index < 0 || index >= length)
        return undefined;
    auto itor = data.cbegin();
    advance(itor, index);
    return *itor;
}
ptrdiff_t JSArray::indexOf(const Any &value, ptrdiff_t fromIndex) const
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
ptrdiff_t JSArray::lastIndexOf(const Any &value, ptrdiff_t fromIndex) const
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
bool JSArray::includes(const Any &value) const
{
    return ::find(data.cbegin(), data.cend(), value) != data.cend();
}
string JSArray::join(const string &seperator) const
{
    stringstream ss;
    size_t crtIndex = 0;
    for (const Any &val : data)
    {
        ss << val << (crtIndex++ == length - 1 ? "" : seperator);
    }
    return ss.str();
}
void JSArray::forEach(const CallbackF_Basic<void> &callbackFn)
{
    auto f = convert_basic<void>(callbackFn);
    size_t crtIndex = 0;
    for (Any val : data)
        f(val, crtIndex++, *this);
}
bool JSArray::some(const CallbackF_Basic<bool> &predicate)
{
    auto f = convert_basic<bool>(predicate);
    size_t crtIndex = 0;
    for (Any val : data)
    {
        if (f(val, crtIndex++, *this))
            return true;
    }
    return false;
}
bool JSArray::every(const CallbackF_Basic<bool> &predicate)
{
    auto f = convert_basic<bool>(predicate);
    size_t crtIndex = 0;
    for (Any val : data)
    {
        if (!f(val, crtIndex++, *this))
            return false;
    }
    return true;
}
Any JSArray::find(const CallbackF_Basic<bool> &predicate)
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
Any JSArray::findLast(const CallbackF_Basic<bool> &predicate)
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
ptrdiff_t JSArray::findIndex(const CallbackF_Basic<bool> &predicate)
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
ptrdiff_t JSArray::findLastIndex(const CallbackF_Basic<bool> &predicate)
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
Any JSArray::reduce(const CallbackF_Reduce &callbackFn, const optional<Any> &_initialValue)
{
    if (length == 0)
        return undefined;
    auto f = convert_reduce(callbackFn);
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
        accumulator = f(accumulator, *itor, crtIndex++, *this);
    }
    return accumulator;
}
Any JSArray::reduceRight(const CallbackF_Reduce &callbackFn, const optional<Any> &_initialValue)
{
    if (length == 0)
        return undefined;
    auto f = convert_reduce(callbackFn);
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
        accumulator = f(accumulator, *itor, crtIndex--, *this);
    }
    return accumulator;
}
JSArray JSArray::filter(const CallbackF_Basic<bool> &predicate)
{
    auto f = convert_basic<bool>(predicate);
    JSArray filterArray(0);
    size_t crtIndex = 0;
    for (Any val : data)
    {
        if (f(val, crtIndex++, *this))
            filterArray.push(val);
    }
    return filterArray;
}
JSArray JSArray::map(const CallbackF_Basic<Any> &mapFn)
{
    auto f = convert_basic<Any>(mapFn);
    JSArray mapArray(0);
    size_t crtIndex = 0;
    for (Any val : data)
        mapArray.push(std::move(f(val, crtIndex++, *this)));
    return mapArray;
}
JSArray JSArray::concat(const JSArray &otherArray) const
{
    JSArray concatArray{*this};
    for (const Any &ele : otherArray.data)
        concatArray.push(ele);
    return concatArray;
}
JSArray JSArray::slice(ptrdiff_t start, optional<ptrdiff_t> _end) const
{
    ptrdiff_t end;
    _end && (end = _end.value()) || (end = length - 1);
    (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
    (end < 0 && end > -length) && (end += length);
    end >= length && (end = length - 1);
    if (start >= length || end <= start)
        return JSArray(0);
    JSArray sliceArray(end - start);
    auto sitor = data.cbegin();
    auto eitor = data.cbegin();
    advance(sitor, start);
    advance(eitor, end);
    copy(sitor, eitor, sliceArray.data.begin());
    return sliceArray;
}
JSArray JSArray::splice(ptrdiff_t start, std::optional<size_t> _deleteCount, const std::initializer_list<Any> &newElements)
{
    JSArray spliceArray(0);
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
JSArray &JSArray::reverse()
{
    data.reverse();
    return *this;
}
JSArray &JSArray::sort(const optional<function<ptrdiff_t(Any, Any)>> &_compareFn)
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
JSArray &JSArray::copyWithin(ptrdiff_t target, ptrdiff_t start, optional<ptrdiff_t> _end)
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
JSArray &JSArray::fill(const Any &value, ptrdiff_t start, optional<ptrdiff_t> _end)
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
Any &JSArray::operator[](size_t index)
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
JSArray &JSArray::operator=(const JSArray &newArray)
{
    if (this != &newArray)
    {
        list<Any>(newArray.data).swap(data);
        *const_cast<size_t *>(&length) = newArray.length;
    }
    return *this;
}
JSArray &JSArray::operator=(JSArray &&newArray)
{
    *const_cast<size_t *>(&length) = std::__exchange(*const_cast<size_t *>(&newArray.length), 0);
    data = std::__exchange(newArray.data, list<Any>{});
    return *this;
}
ostream &operator<<(ostream &os, const JSArray &JSarray)
{
    if (JSarray.length > 1)
        os << "\n(" << JSarray.length << ") ";
    os << "[";
    size_t crtIndex = 0;
    for (const Any &val : JSarray.data)
    {
        os << val << (crtIndex++ == JSarray.length - 1 ? "" : ", ");
    }
    os << "]\n";
    return os;
}