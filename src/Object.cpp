#include "../inc/Object.h"
using namespace std;

//////////////////////////////////Operator
Any &Object::operator[](string &&key)
{
    if (key == "0" ||
        key.find('.') == string::npos && atoll(key.c_str()) > 0)
        return this->operator[](atoll(key.c_str()));
    if (k_v.find(key) == k_v.end())
    {
        key_str.push_back(key);
    }
    return k_v[key];
}
Any &Object::operator[](ptrdiff_t &&_key)
{
    string key = to_string(_key);
    if (_key < 0)
        return this->operator[](move(key));
    if (k_v.find(key) == k_v.end())
    {
        key_num.push_back(_key);
        key_num.sort();
    }
    return k_v[key];
}
ostream &operator<<(ostream &os, const Object &o)
{
    os << "\n{";
    for (auto itor = o.key_num.begin(); itor != o.key_num.end(); itor++)
    {
        os << *itor << ": " << o.k_v.at(to_string(*itor)) << (distance(itor, o.key_num.end()) == 1 ? (o.key_str.empty() ? "" : ", ") : ", ");
    }
    for (auto itor = o.key_str.begin(); itor != o.key_str.end(); itor++)
    {
        os << *itor << ": " << o.k_v.at(*itor) << (distance(itor, o.key_str.end()) == 1 ? "" : ", ");
    }
    os << "}\n";
    return os;
}
bool Object::_delete(variant<ptrdiff_t, string> &&_key)
{
    if (_key.index() == 0)
    {
        string key = to_string(get<0>(_key));
        if (k_v.erase(key) > 0)
            key_num.erase(find(key_num.begin(), key_num.end(), get<0>(_key)));
    }
    else
    {
        string key = get<1>(_key);
        if (k_v.erase(key) > 0)
            key_str.erase(find(key_str.begin(), key_str.end(), key));
    }
    return true;
}
//////////////////////////////////Static Methods
ArrayLike<Any> Object::keys(const Object &o)
{
    ArrayLike<Any> keys(0);
    for (auto itor = o.key_num.begin(); itor != o.key_num.end(); itor++)
    {
        keys.push(to_string(*itor));
    }
    for (auto itor = o.key_str.begin(); itor != o.key_str.end(); itor++)
    {
        keys.push(*itor);
    }
    return keys;
}
ArrayLike<Any> Object::values(const Object &o)
{
    ArrayLike<Any> values(0);
    for (auto itor = o.key_num.begin(); itor != o.key_num.end(); itor++)
    {
        values.push(o.k_v.at(to_string(*itor)));
    }
    for (auto itor = o.key_str.begin(); itor != o.key_str.end(); itor++)
    {
        values.push(o.k_v.at(*itor));
    }
    return values;
}