template <typename T>
class Array
{
private:
    T *data;

public:
    Array(unsigned int size);
    Array(const Array &otherArray);
    /////////////
    typedef void (*callback)(T value, unsigned int index, Array<T> *thisArray);
    void forEach(callback c);
};
