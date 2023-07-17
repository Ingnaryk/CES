#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <vector>

class timer
{
public:
    using millisecond = std::chrono::duration<float, std::milli>;
    inline static int outputBufferSize;
    timer(const char *) noexcept;
    timer(const timer &) = delete;
    timer(timer &&) = delete;
    timer &operator=(const timer &) = delete;
    timer &operator=(timer &&) = delete;
    ~timer();

private:
    inline static std::vector<std::pair<const char *, millisecond>> results;
    std::chrono::time_point<std::chrono::high_resolution_clock> start{}, end{};
    const char *name{nullptr};
};

#endif // _TIMER_H_
