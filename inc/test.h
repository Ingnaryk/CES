#ifndef _TEST_H_
#define _TEST_H_

namespace test
{
    struct testType
    {
        static int ctor, cctor, mctor, cass, mass, dtor;
        int data{0};
        testType() { ctor++; }
        testType(int data) : data{data} { ctor++; }
        testType(const testType &o) : data{o.data} { cctor++; }
        testType(testType &&ro)
        {
            std::swap(data, ro.data);
            mctor++;
        }
        testType &operator=(const testType &o)
        {
            cass++;
            data = o.data;
            return *this;
        }
        testType &operator=(testType &&ro)
        {
            mass++;
            std::swap(data, ro.data);
            return *this;
        }
        ~testType()
        {
            data = 0;
            dtor++;
        }
    };

    void demo_StdArray_RawArray_RawList();

    void test_RawList_stdlist();

    void test_RawArray_stdvector();

    void testAsync();

    void testString();
} // namespace test

#endif // _TEST_H
