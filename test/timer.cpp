#include <iostream>
#include <algorithm>

#include "test/timer.h"

timer::timer(const char *name) noexcept : name{name}
{
    start = std::chrono::high_resolution_clock::now();
}

timer::~timer()
{
    constexpr auto sorter = [](auto &a, auto &b) -> bool
    { return a.second < b.second; };
    end = std::chrono::high_resolution_clock::now();
    timer::results.emplace_back(name, end - start);
    if (timer::results.size() >= timer::outputBufferSize)
    {
        std::sort(timer::results.begin(), timer::results.end(), sorter);
        for (auto &[name, time] : timer::results)
            std::cout << name << " used " << time << std::endl;
        timer::results.clear();
    }
}
