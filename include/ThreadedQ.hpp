#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>

template <typename T>
class ThreadedQ
{
public:
    ThreadedQ() = default;
    ThreadedQ(const size_t size): q(size) {}

    ThreadedQ(const ThreadedQ<T> &) = delete;
    ThreadedQ(const ThreadedQ<T> &&) = delete;
    ThreadedQ &operator=(const ThreadedQ &) = delete;

    virtual ~ThreadedQ() { this->clear(); }

    inline bool empty() const
    {
        std::scoped_lock lock(q_mut);
        return q.empty();
    }

    inline auto begin()
    {
        std::scoped_lock lock(q_mut);
        return q.begin();
    }

    inline auto end()
    {
        std::scoped_lock lock(q_mut);
        return q.end();
    }

    inline auto size() const
    {
        std::scoped_lock lock(q_mut);
        return q.size();
    }

    inline auto erase(auto i, auto e = {})
    {
        std::scoped_lock lock(q_mut);
        q.erase(i, e);
    }

    inline void clear()
    {
        std::scoped_lock lock(q_mut);
        q.clear();
    }
    inline std::optional<T> pop_front()
    {
        std::scoped_lock lock(q_mut);
        if (q.size() == 0) return std::nullopt;
        T t = std::move(q.front());
        q.pop_front();
        return t;
    }
    inline std::optional<T> pop_back()
    {
        std::scoped_lock lock(q_mut);
        if (q.size() == 0) return std::nullopt;
        T t = std::move(q.front());
        q.pop_back();
        return t;
    }

    template <typename... Args>
    requires std::is_constructible_v<T, Args...>
    inline void emplace_back(Args... args)
    {
        auto t = T(args...);
        this->push_back(t);
    }

    template <typename... Args>
    requires std::is_constructible_v<T, Args...>
    inline void emplace_fron(Args... args)
    {
        auto t = T(args...);
        this->push_front(t);
    }

    inline void push_back(const T &i)
    {
        std::scoped_lock lock(q_mut);
        q.emplace_back(std::move(i));

        std::unique_lock<std::mutex> ul(mutBlocking);
        vBlocking.notify_one();
    }
    inline void push_front(const T &i)
    {
        std::scoped_lock lock(q_mut);
        q.emplace_front(std::move(i));

        std::unique_lock<std::mutex> ul(mutBlocking);
        vBlocking.notify_one();
    }

    inline T &back()
    {
        std::scoped_lock lock(q_mut);
        return q.back();
    }
    inline T &front()
    {
        std::scoped_lock lock(q_mut);
        return q.front();
    }

    inline void wait()
    {
        while (this->bWait && this->empty()) {
            std::unique_lock<std::mutex> ul(mutBlocking);
            vBlocking.wait(ul);
        }
    }
    template <unsigned D = 10>
    inline void waitTimeout()
    {
        std::unique_lock<std::mutex> ul(mutBlocking);
        vBlocking.wait_for(ul, std::chrono::milliseconds(D));
    }

    inline void waitTimeout(unsigned d = 10)
    {
        std::unique_lock<std::mutex> ul(mutBlocking);
        vBlocking.wait_for(ul, std::chrono::milliseconds(d));
    }

    inline void notify()
    {
        std::unique_lock<std::mutex> ul(mutBlocking);
        vBlocking.notify_all();
    }

    inline void setWaitMode(bool mode)
    {
        bWait = mode;
        this->notify();
    }

private:
    mutable std::mutex q_mut;
    std::deque<T> q;

    std::atomic_bool bWait = true;
    std::mutex mutBlocking;
    std::condition_variable vBlocking;
};
