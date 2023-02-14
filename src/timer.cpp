#include <iostream>

#include "timer.h"

int timer::outputBuffer{};
std::map<const char *, timer::millisecond> timer::recentResults{};

timer::timer(const char *name) : name(name)
{
    start = std::chrono::high_resolution_clock::now();
}

timer::~timer()
{
    end = std::chrono::high_resolution_clock::now();
    recentResults.emplace(name, end - start);
    if (recentResults.size() >= outputBuffer)
    {
        for (auto &[name, time] : recentResults)
            std::cout << name << " cost " << time << std::endl;
        recentResults.clear();
    }
}
