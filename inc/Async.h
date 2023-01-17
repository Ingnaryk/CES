#ifndef _ASYNC_H_
#define _ASYNC_H_

#include <thread>
#include <future>

#include "Any.h"

static std::vector<std::jthread> threadPool;

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
    AsyncFunction(std::function<R(Args...)> &&func) : func(std::move(func)) {}
    Promise<R> operator()(Args... args)
    {
        std::future<R> future = std::async(this->func, args...);
        return Promise<R>(std::move(future));
    }
};

template <typename T>
class Promise
{
private:
    std::future<T> m_p;

public:
    Promise(std::future<T> future) : m_p(std::move(future)) {}
    void then(const std::function<void(T)> &callback)
    {
        std::jthread t(callback, m_p.get());
        threadPool.emplace_back(std::move(t));
    }
};

#endif // _ASYNC_H_