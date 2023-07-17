#ifndef _ASYNC_H_
#define _ASYNC_H_

#include <thread>
#include <future>
#include <vector>
#include <map>

using namespace std::chrono_literals;

class ThreadPool
{
private:
    std::vector<std::jthread> threads;
    std::map<size_t, std::stop_source> stoppersMap;
    void clearTimer(size_t timerId)
    {
        auto stopper = stoppersMap.find(timerId);
        if (stopper == stoppersMap.end())
            return;
        stopper->second.request_stop();
        stoppersMap.erase(timerId);
    }

public:
    ThreadPool() noexcept = default;
    ThreadPool(const ThreadPool &) = delete;
    template <typename... Args>
    constexpr void submit(Args &&...args) { threads.emplace_back(std::forward<Args>(args)...); }
    template <typename... Args>
    size_t submitTimer(Args &&...args)
    {
        std::jthread &t = threads.emplace_back(std::forward<Args>(args)...);
        size_t stopId = std::hash<std::jthread::id>{}(t.get_id());
        std::stop_source stopper = t.get_stop_source();
        stoppersMap.emplace(stopId, std::move(stopper));
        return stopId;
    }
    friend inline void clearTimeout(size_t timerId);
    friend inline void clearInterval(size_t timerId);
};

inline ThreadPool threadPool;

template <typename R, typename... Args>
class AsyncFunction;
template <typename T>
class Promise;

template <typename R, typename... Args>
class AsyncFunction
{
private:
    std::function<R(Args...)> _func;

public:
    AsyncFunction(std::function<R(Args...)> &&func) : _func{std::move(func)} {}
    Promise<R> operator()(Args &&...args)
    {
        return Promise<R>{std::async(this->_func, std::forward<Args>(args)...)};
    }
};

template <typename R>
class Promise
{
private:
    std::future<R> _future;

public:
    Promise(std::future<R> &&future) : _future{std::move(future)} {}
    void then(const std::function<void(R)> &callback)
    {
        threadPool.submit([future = std::move(this->_future)](const auto &callback) mutable
                          { callback(future.get()); },
                          callback);
    }
};

/**
 * @brief delay the execution of the function for a period of time, support passing other arguments
 *
 * @param handler the function for execution
 * @param timeout the delay time as millisecond in integar (use c++ chrono presentation)
 * @param arguments arguments passed when the @c handler is called
 *
 * @return the unique identification of the timer which can be terminated by function @c clearTimeout
 */
template <typename Callable, typename... Args>
size_t setTimeout(Callable &&handler, std::chrono::milliseconds timeout = {}, Args &&...arguments)
{
    return threadPool.submitTimer([timeout, exec = std::bind(std::forward<Callable>(handler), std::forward<Args>(arguments)...)](std::stop_token stok)
                                  {
                                      std::this_thread::sleep_for(timeout);
                                      if (stok.stop_requested())
                                          return;
                                      exec(); });
}

/**
 * @brief terminate a timer, also valid to timer created by function @c setInterval
 *
 * @param timerId the id of timer to be terminated
 */
inline void clearTimeout(size_t timerId)
{
    threadPool.clearTimer(timerId);
}

/**
 * @brief Periodic execution of function, supports passing other arguments. Pay attention that the timer thread
 * created by this function won't stop after @c main function returns.
 *
 * @param handler the function for execution
 * @param timeout the delay time as millisecond in integar (use c++ chrono presentation)
 * @param arguments arguments passed when the @c handler is called
 *
 * @return the unique identification of the timer which can be terminated by function @c clearInterval
 */
template <typename Callable, typename... Args>
size_t setInterval(Callable &&handler, std::chrono::milliseconds timeout = {}, Args &&...arguments)
{
    return threadPool.submitTimer([timeout, exec = std::bind(std::forward<Callable>(handler), std::forward<Args>(arguments)...)](std::stop_token stok)
                                  {
                                      while (!stok.stop_requested())
                                      {
                                          std::this_thread::sleep_for(timeout);
                                          exec();
                                      } });
}

/**
 * @brief terminate a periodic timer, also valid to timer created by function @c setTimeout
 *
 * @param timerId the id of timer to be terminated
 */
inline void clearInterval(size_t timerId)
{
    threadPool.clearTimer(timerId);
}

#endif // _ASYNC_H_
