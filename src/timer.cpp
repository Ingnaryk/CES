#include <iostream>

#include "timer.h"

timer::timer(const char *name) : name(name)
{
    start = std::chrono::high_resolution_clock::now();
}

timer::~timer()
{
    end = std::chrono::high_resolution_clock::now();
    std::cout << name << " costs " << millisecond(end - start) << std::endl;
}
