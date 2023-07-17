#ifndef _MAP_H_
#define _MAP_H_

#include <map>

#include "CES/Array.h"

namespace CES
{
    template <typename Key, typename Value>
    class Map
    {
    private:
        //////////////////////////////////Internal usage
        template <typename R, typename ESCallback>
        constexpr auto basicAdaptor(ESCallback &&func)
        {
            return [&, index = 0LL](const std::pair<Key, Value> &value) mutable -> R
            {
                if constexpr (std::is_invocable_r_v<R, ESCallback, Value>)
                    return func(value.second);
                else if constexpr (std::is_invocable_r_v<R, ESCallback, Value, Key>)
                    return func(value.second, value.first);
                else if constexpr (std::is_invocable_r_v<R, ESCallback, Value, Key, Map &>)
                    return func(value.second, value.first, *this);
            };
        }
        //////////////////////////////////Wrapped data
        std::map<Key, Value> data;

    public:
        //////////////////////////////////ES Property
        const ptrdiff_t size{0};
        //////////////////////////////////Constructor
        Map() : data{} {}
        Map(std::initializer_list<std::pair<Key, Value>> list) : data(list) {}
        Map(const Map &map) : data(map.data) {}
        Map(Map &&map) : data(std::move(map.data)) {}
        //////////////////////////////////ES Method
        constexpr any get(const Key &key)
        {
            if (data.find(key) == data.end())
                return undefined;
            return data.at(key);
        }
        constexpr void set(Key &&key, const Value &val)
        {
            if (data.find(key) == data.end())
                const_cast<ptrdiff_t &>(size) += 1;
            data[key] = val;
        }
        constexpr bool Delete(const Key &key) // avoid using keyword 'delete' reserved in C++
        {
            bool deleted = (data.erase(key) > 0);
            const_cast<ptrdiff_t &>(size) -= deleted;
            return deleted;
        }
        constexpr void clear()
        {
            data.clear();
            const_cast<ptrdiff_t &>(size) = 0;
        }
        constexpr bool has(const Key &key)
        {
            return data.find(key) != data.end();
        }
        template <typename ESCallback>
        constexpr void forEach(ESCallback &&callbackFn)
        {
            std::for_each(data.begin(), data.end(), basicAdaptor<void>(std::forward<ESCallback>(callbackFn)));
        }
        constexpr Array<Key> keys()
        {
            auto keysRange = data | std::views::keys;
            return Array<Key>{keysRange.begin(), keysRange.end()};
        }
        constexpr Array<Value> values()
        {
            auto valuesRange = data | std::views::values;
            return Array<Value>{valuesRange.begin(), valuesRange.end()};
        }
        constexpr Array<std::pair<Key, Value>> entries()
        {
            auto entriesRange = data | std::views::all;
            return Array<std::pair<Key, Value>>{entriesRange.begin(), entriesRange.end()};
        }
        //////////////////////////////////Operator
        friend constexpr std::ostream &operator<<(std::ostream &os, const Map<Key, Value> &map)
        {
            os << "\nMap(" << map.size << ") {";
            if (map.size == 0)
                os << "size: 0";
            else
            {
                ptrdiff_t crtIndex = 0;
                for (auto &[key, value] : map.data)
                    os << key << " => " << value << (++crtIndex == map.size ? "" : ", ");
            }
            os << "}\n";
            return os;
        }
    };
} // CES

#endif // _MAP_H_