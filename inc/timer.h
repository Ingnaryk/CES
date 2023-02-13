#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>

struct timer
{
    using millisecond = std::chrono::duration<float, std::milli>;
    const char *name;
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    Timer(const char *);
    Timer(const Timer &) = delete;
    ~Timer();
};

#endif // _TIMER_H_
