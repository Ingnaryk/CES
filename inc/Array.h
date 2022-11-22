#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <list>
#include "Any.h"

class Array
{
    template <typename U>
    using CallbackF = std::variant<std::function<U()>,
                                   std::function<U(Any)>,
                                   std::function<U(Any, size_t)>,
                                   std::function<U(Any, size_t, Array &)>>;
    template <typename newT>
    std::function<newT(Any, size_t, Array &)> convert(const CallbackF<newT> &fn)
    {
        std::function<newT(Any, size_t, Array &)> f;
        switch (fn.index())
        {
        case 0:
            f = [&fn](Any, size_t, Array &)
            { return std::get<0>(fn)(); };
            break;
        case 1:
            f = [&fn](Any value, size_t, Array &)
            { return std::get<1>(fn)(value); };
            break;
        case 2:
            f = [&fn](Any value, size_t index, Array &)
            { return std::get<2>(fn)(value, index); };
            break;
        case 3:
            f = [&fn](Any value, size_t index, Array &thisArray)
            { return std::get<3>(fn)(value, index, thisArray); };
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
    Array() noexcept = default;
    explicit Array(size_t);
    Array(const std::initializer_list<Any> &);
    Array(const Array &);
    Array(Array &&) noexcept;
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

    void forEach(const CallbackF<void> &);
    bool some(const CallbackF<bool> &);
    bool every(const CallbackF<bool> &);
    Any find(const CallbackF<bool> &);
    Any findLast(const CallbackF<bool> &);
    ptrdiff_t findIndex(const CallbackF<bool> &);
    ptrdiff_t findLastIndex(const CallbackF<bool> &);
    Any reduce(const std::function<Any(Any, Any, size_t, Array *)> &, const std::optional<Any> &_initialValue = std::nullopt);
    Any reduceRight(const std::function<Any(Any, Any, size_t, Array *)> &, const std::optional<Any> &_initialValue = std::nullopt);

    Array filter(const CallbackF<bool> &);
    Array map(const CallbackF<Any> &);
    Array concat(const Array &) const;
    Array slice(ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt) const;
    Array splice(ptrdiff_t target = 0, std::optional<size_t> _deleteCount = std::nullopt, const std::initializer_list<Any> &newElements = {});
    Array &reverse();
    Array &sort(const std::optional<std::function<ptrdiff_t(Any, Any)>> &_compareFn = std::nullopt);
    Array &copyWithin(ptrdiff_t, ptrdiff_t, std::optional<ptrdiff_t> _end = std::nullopt);
    Array &fill(const Any &, ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt);
    //////////////////////////////////Operator
    Any &operator[](size_t);
    Array &operator=(const Array &);
    Array &operator=(Array &&);
    friend std::ostream &operator<<(std::ostream &, const Array &);
};

#endif //_ARRAY_H_