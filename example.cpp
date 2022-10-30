#include <stdio.h>
#include "Array.h"

int main()
{
    Array<int> a = {1, 3, 5, 8, 9};
    a.forEach([](int value, size_t index, ...)
              { printf("%u: %d\n", index, value); });
    if (a.some([](int value, ...)
               { return value % 2 == 0; }))
        printf("a includes even number\n");
    if (a.every([](int value, ...)
                { return value > 0; }))
        printf("all the numbers in a are positive\n");
    Array<double> b = a.map<double>([](int value, ...)
                                    { return value * 0.3; });
    b.forEach([](double value, size_t index, ...)
              { printf("%u: %lf\n", index, value); });
    return 0;
}