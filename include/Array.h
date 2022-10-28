template <typename T>
class Array
{
private:
    T *data;

public:
    Array(unsigned int size);
    Array(const Array &otherArray);
    /////////////
    typedef void (*callbackv)(T value, unsigned int index, Array<T> *thisArray);
    typedef bool (*callbackb)(T value, unsigned int index, Array<T> *thisArray);
    void forEach(callbackv cv);
    bool some(callbackb cb);
};
