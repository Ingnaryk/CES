#ifndef _JSARRAY_H_
#define _JSARRAY_H_

#include <list>
#include "Any.h"

class JSArray
{
    template <typename U>
    using CallbackF_Basic = std::variant<std::function<U()>,
                                         std::function<U(Any)>,
                                         std::function<U(Any, size_t)>,
                                         std::function<U(Any, size_t, JSArray &)>>;
    using CallbackF_Reduce = std::variant<std::function<Any(Any, Any)>,
                                          std::function<Any(Any, Any, size_t)>,
                                          std::function<Any(Any, Any, size_t, JSArray &)>>;
    template <typename newT>
    std::function<newT(Any, size_t, JSArray &)> convert_basic(const CallbackF_Basic<newT> &fn)
    {
        std::function<newT(Any, size_t, JSArray &)> f;
        switch (fn.index())
        {
        case 0:
            f = [&fn](Any, size_t, JSArray &)
            { return std::get<0>(fn)(); };
            break;
        case 1:
            f = [&fn](Any value, size_t, JSArray &)
            { return std::get<1>(fn)(value); };
            break;
        case 2:
            f = [&fn](Any value, size_t index, JSArray &)
            { return std::get<2>(fn)(value, index); };
            break;
        case 3:
            f = [&fn](Any value, size_t index, JSArray &thisArray)
            { return std::get<3>(fn)(value, index, thisArray); };
            break;
        }
        return f;
    }
    std::function<Any(Any, Any, size_t, JSArray &)> convert_reduce(const CallbackF_Reduce &fn)
    {
        std::function<Any(Any, Any, size_t, JSArray &)> f;
        switch (fn.index())
        {
        case 0:
            f = [&fn](Any value1, Any value2, size_t, JSArray &)
            { return std::get<0>(fn)(value1, value2); };
            break;
        case 1:
            f = [&fn](Any value1, Any value2, size_t crtIndex, JSArray &)
            { return std::get<1>(fn)(value1, value2, crtIndex); };
            break;
        case 2:
            f = [&fn](Any value1, Any value2, size_t crtIndex, JSArray &thisArray)
            { return std::get<2>(fn)(value1, value2, crtIndex, thisArray); };
            break;
        }
        return f;
    }

private:
    std::list<Any> data{};

public:
    //////////////////////////////////Variadic templates functions' end
    template <typename Arg>
    size_t push(const Arg &element)
    {
        data.push_back(element);
        *const_cast<size_t *>(&length) = length + 1;
        return length;
    }
    template <typename Arg>
    size_t unshift(const Arg &element)
    {
        data.push_front(element);
        *const_cast<size_t *>(&length) = length + 1;
        return length;
    }
    //////////////////////////////////ES Properties
    const size_t length{0};
    //////////////////////////////////Constructor
    JSArray() noexcept = default;
    explicit JSArray(size_t);
    JSArray(const std::initializer_list<Any> &);
    JSArray(const JSArray &);
    JSArray(JSArray &&) noexcept;
    //////////////////////////////////ES Functions
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
    Any pop();
    Any shift();
    Any at(ptrdiff_t) const;
    ptrdiff_t indexOf(const Any &, ptrdiff_t fromIndex = 0) const;
    ptrdiff_t lastIndexOf(const Any &, ptrdiff_t fromIndex = -1) const;
    bool includes(const Any &) const;
    std::string join(const std::string &seperator = ",") const;

    void forEach(const CallbackF_Basic<void> &);
    bool some(const CallbackF_Basic<bool> &);
    bool every(const CallbackF_Basic<bool> &);
    Any find(const CallbackF_Basic<bool> &);
    Any findLast(const CallbackF_Basic<bool> &);
    ptrdiff_t findIndex(const CallbackF_Basic<bool> &);
    ptrdiff_t findLastIndex(const CallbackF_Basic<bool> &);
    Any reduce(const CallbackF_Reduce &, const std::optional<Any> &_initialValue = std::nullopt);
    Any reduceRight(const CallbackF_Reduce &, const std::optional<Any> &_initialValue = std::nullopt);

    JSArray filter(const CallbackF_Basic<bool> &);
    JSArray map(const CallbackF_Basic<Any> &);
    JSArray concat(const JSArray &) const;
    JSArray slice(ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt) const;
    JSArray splice(ptrdiff_t target = 0, std::optional<size_t> _deleteCount = std::nullopt, const std::initializer_list<Any> &newElements = {});
    JSArray &reverse();
    JSArray &sort(const std::optional<std::function<ptrdiff_t(Any, Any)>> &_compareFn = std::nullopt);
    JSArray &copyWithin(ptrdiff_t, ptrdiff_t, std::optional<ptrdiff_t> _end = std::nullopt);
    JSArray &fill(const Any &, ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt);
    //////////////////////////////////Operator
    Any &operator[](size_t);
    JSArray &operator=(const JSArray &);
    JSArray &operator=(JSArray &&);
    friend std::ostream &operator<<(std::ostream &, const JSArray &);
};

#endif //_JSARRAY_H_