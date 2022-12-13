#ifndef _ASYNC_H_
#define _ASYNC_H_

#include <thread>
#include <future>
#include "Any.h"

using ResExecutor = void(Any);

template <typename R, typename... Args>
class AsyncFunction;
template <typename T>
class Promise;

template <typename R, typename... Args>
class AsyncFunction
{
private:
    std::optional<std::future<R>> future{std::nullopt};
    std::function<R(Args...)> func;

public:
    AsyncFunction(const std::function<R(Args...)> &func) : func(func) {}
    Promise<R> operator()(Args... args)
    {
        std::promise<R> promise;
        std::thread t([this]()
                      { this->func(args...);
                       promise.set_value });
    }
};

template <typename T>
class Promise
{
private:
    std::promise<T> m_p;

public:
    Promise(std::promise<T> &&promise) : m_p(std::move(promise)) {}
};

#endif