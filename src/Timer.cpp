#include <iostream>

#include "Timer.h"

Timer::Timer(const char *name) : name(name)
{
    start = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
    end = std::chrono::high_resolution_clock::now();
    std::cout << name << " costs " << millisecond(end - start) << std::endl;
}