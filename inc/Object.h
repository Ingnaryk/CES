#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <map>
#include <list>
#include "Any.h"
#include "ArrayLike.h"

class Object
{
private:
    std::map<std::string, Any> k_v{};
    std::list<std::string> key_str{};
    std::list<size_t> key_num{};

public:
    //////////////////////////////////Constructor
    Object() noexcept = default;
    //////////////////////////////////Operator
    Any &operator[](std::string &&);
    Any &operator[](ptrdiff_t &&);
    friend std::ostream &operator<<(std::ostream &, const Object &);
    bool _delete(std::variant<ptrdiff_t, std::string> &&); /*cannot use 'delete' keyword here*/
    //////////////////////////////////Static Methods
    static ArrayLike<Any> keys(const Object &);
    static ArrayLike<Any> values(const Object &);
};

#endif //_OBJECT_H_