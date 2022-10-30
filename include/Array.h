#include <functional>
#include <initializer_list>

template <typename T>
class Array
{
private:
    T *data{nullptr};
    size_t length{0};

public:
    //////////////////////////////////Constructor
    Array(size_t size)
    {
        length = size;
        data = new T[length];
    }
    Array(const std::initializer_list<T> &il)
    {
        length = il.size();
        data = new T[length];
        typename std::initializer_list<T>::iterator itor;
        for (itor = il.begin(); itor != il.end(); itor++)
            data[itor - il.begin()] = *itor;
    }
    Array(const Array<T> &otherArray)
    {
        length = otherArray.length;
        data = new T[length];
        for (size_t i = 0; i < length; i++)
        {
            data[i] = otherArray.data[i];
        }
    }
    //////////////////////////////////ES Functions
    void forEach(std::function<void(T, size_t, Array<T> &)> callback)
    {
        for (size_t i = 0; i < length; i++)
        {
            callback(data[i], i, *this);
        }
    }
    bool some(std::function<bool(T, size_t, Array<T> &)> callback)
    {
        for (size_t i = 0; i < length; i++)
        {
            if (callback(data[i], i, *this))
                return true;
        }
        return false;
    }
    bool every(std::function<bool(T, size_t, Array<T> &)> callback)
    {
        for (size_t i = 0; i < length; i++)
        {
            if (!callback(data[i], i, *this))
                return false;
        }
        return true;
    }
    template <typename newT>
    Array<newT> map(std::function<newT(T, size_t, Array<T> &)> callback)
    {
        Array<newT> tmp(length);
        for (size_t i = 0; i < length; i++)
        {
            tmp[i] = callback(data[i], i, *this);
        }
        return tmp;
    }
    //////////////////////////////////Operator
    T &operator[](size_t index)
    {
        return data[index];
    }
    //////////////////////////////////Destructor
    ~Array()
    {
        if (data != nullptr)
            delete data;
    }
};
