#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>

using namespace std::chrono_literals;

struct timer
{
    using millisecond = std::chrono::duration<float, std::milli>;
    const char *name;
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    timer(const char *);
    timer(const timer &) = delete;
    ~timer();
};

#endif // _TIMER_H_
