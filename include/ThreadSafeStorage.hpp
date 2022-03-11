#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <optional>

template <class T, class A = std::allocator<T>>
class ThreadSafeStorage
{
    using size_t = typename A::size_type;

    class iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = size_t;
        using pointer = T *;
        using reference = T &;

    public:
        iterator(ThreadSafeStorage &s, const size_t start = 0): to(s.size()), from(0), index(start), s(s){};

        constexpr std::strong_ordering operator<=>(const iterator &) const noexcept = default;
        constexpr auto operator==(const iterator &other) const noexcept { return index == other.index; };
        constexpr auto operator!=(const iterator &other) const noexcept { return index != other.index; };
        constexpr iterator &operator++() noexcept
        {
            index++;
            return *this;
        }
        constexpr iterator operator++(int i) noexcept
        {
            iterator tmp = *tmp;
            (*this) += i;
            return tmp;
        }

        constexpr T &operator*() const { return s.at(index); }
        constexpr T *operator->() const { return s.at(index); }

    private:
        const size_t to;
        const size_t from;
        size_t index;
        ThreadSafeStorage &s;
    };
    // Disable because of defect P2325R3
    // static_assert(std::input_iterator<typename ThreadSafeStorage<T>::iterator>);

public:
    ThreadSafeStorage() = default;
    ThreadSafeStorage(const size_t size): q(size) {}

    ThreadSafeStorage(const ThreadSafeStorage<T> &) = delete;
    ThreadSafeStorage(const ThreadSafeStorage<T> &&) = delete;
    ThreadSafeStorage &operator=(const ThreadSafeStorage &) = delete;

    ~ThreadSafeStorage() { this->clear(); }

    bool empty() const
    {
        std::scoped_lock lock(q_mut);
        return q.empty();
    }

    auto begin() { return iterator(*this, 0); }

    auto end() { return iterator(*this, this->size()); }

    auto size() const
    {
        std::scoped_lock lock(q_mut);
        return q.size();
    }

    void erase(std::function<bool(const T &i)> &&t)
    {
        std::scoped_lock lock(q_mut);
        auto e = std::remove_if(q.begin(), q.end(), t);
        q.erase(e, q.end());
    }

    void clear()
    {
        std::scoped_lock lock(q_mut);
        q.clear();
    }

    const T &at(const size_t &index) const
    {
        std::scoped_lock lock(q_mut);
        return q.at(index);
    }

    T &at(const size_t &index)
    {
        std::scoped_lock lock(q_mut);
        return q.at(index);
    }

    std::optional<T> pop_front()
    {
        std::scoped_lock lock(q_mut);
        if (q.size() == 0) return std::nullopt;
        T t = std::move(q.front());
        q.pop_front();
        return t;
    }
    std::optional<T> pop_back()
    {
        std::scoped_lock lock(q_mut);
        if (q.size() == 0) return std::nullopt;
        T t = std::move(q.front());
        q.pop_back();
        return t;
    }

    template <typename... Args>
    requires std::is_constructible_v<T, Args...>
    void emplace_back(Args... args)
    {
        auto t = T(args...);
        this->push_back(t);
    }

    template <typename... Args>
    requires std::is_constructible_v<T, Args...>
    void emplace_from(Args... args)
    {
        auto t = T(args...);
        this->push_front(t);
    }

    void push_back(const T &i)
    {
        std::scoped_lock lock(q_mut);
        q.emplace_back(std::move(i));

        vBlocking.notify_one();
    }
    void push_front(const T &i)
    {
        std::scoped_lock lock(q_mut);
        q.emplace_front(std::move(i));

        vBlocking.notify_one();
    }

    T &back()
    {
        std::scoped_lock lock(q_mut);
        return q.back();
    }
    T &front()
    {
        std::scoped_lock lock(q_mut);
        return q.front();
    }

    void wait()
    {
        while (this->bWait && this->empty()) {
            std::unique_lock<std::mutex> ul(mutBlocking);
            vBlocking.wait(ul);
        }
    }
    template <unsigned D = 10>
    void waitTimeout()
    {
        std::unique_lock<std::mutex> ul(mutBlocking);
        vBlocking.wait_for(ul, std::chrono::milliseconds(D));
    }

    void waitTimeout(unsigned d = 10)
    {
        std::unique_lock<std::mutex> ul(mutBlocking);
        vBlocking.wait_for(ul, std::chrono::milliseconds(d));
    }

    void notify() { vBlocking.notify_all(); }

    void setWaitMode(bool mode)
    {
        bWait = mode;
        this->notify();
    }

private:
    mutable std::mutex q_mut;
    std::deque<T, A> q;

    std::atomic_bool bWait = true;
    std::mutex mutBlocking;
    std::condition_variable vBlocking;
};
