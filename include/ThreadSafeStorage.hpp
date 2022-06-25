#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <optional>

#define STORAGE_METHOD_IMPL__(ret, name, modifier, impl)  \
    ret name(std::unique_lock<std::mutex> &lock) modifier \
    {                                                     \
        if (!lock.owns_lock()) lock.lock();               \
        impl                                              \
    }                                                     \
    ret name() modifier                                   \
    {                                                     \
        std::unique_lock<std::mutex> lock(q_mut);         \
        return name(lock);                                \
    }

#define STORAGE_METHOD_IMPL(ret, name, impl) STORAGE_METHOD_IMPL__(ret, name, , impl)
#define STORAGE_METHOD_IMPL_CONST(ret, name, impl) STORAGE_METHOD_IMPL__(ret, name, const, impl)
#define STORAGE_METHOD_IMPL_BOTH(ret, name, impl) \
    STORAGE_METHOD_IMPL__(ret, name, , impl) STORAGE_METHOD_IMPL__(const ret, name, const, impl)

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
        constexpr T *operator->() const { return &(s.at(index)); }

    private:
        const size_t to;
        const size_t from;
        size_t index;
        ThreadSafeStorage &s;
    };

public:
    ThreadSafeStorage() = default;
    ThreadSafeStorage(const size_t size): q(size) {}

    ThreadSafeStorage(const ThreadSafeStorage<T> &) = delete;
    ThreadSafeStorage(const ThreadSafeStorage<T> &&) = delete;
    ThreadSafeStorage &operator=(const ThreadSafeStorage &) = delete;

    ~ThreadSafeStorage() { this->clear(); }

    auto begin() { return iterator(*this, 0); }
    auto end() { return iterator(*this, this->size()); }

    void erase(std::unique_lock<std::mutex> &lock, const std::function<bool(const T &i)> &&t)
    {
        if (!lock.owns_lock()) lock.lock();
        auto e = std::remove_if(q.begin(), q.end(), t);
        q.erase(e, q.end());
    }
    void erase(const std::function<bool(const T &i)> &&t)
    {
        std::unique_lock<std::mutex> lock(q_mut);
        erase(lock, std::move(t));
    }
    STORAGE_METHOD_IMPL(void, clear, { q.clear(); });
    STORAGE_METHOD_IMPL(std::optional<T>, pop_front, {
        if (size(lock) == 0) return std::nullopt;
        T t = std::move(front(lock));
        q.pop_front();
        return t;
    })
    STORAGE_METHOD_IMPL(std::optional<T>, pop_back, {
        if (size(lock) == 0) return std::nullopt;
        T t = std::move(front(lock));
        q.pop_back();
        return t;
    })
    STORAGE_METHOD_IMPL_CONST(bool, empty, { return q.empty(); });
    STORAGE_METHOD_IMPL_CONST(auto, size, { return q.size(); });

    STORAGE_METHOD_IMPL_BOTH(T &, back, { return q.back(); })
    STORAGE_METHOD_IMPL_BOTH(T &, front, { return q.front(); })

    const T &at(std::unique_lock<std::mutex> &lock, const size_t &index) const
    {
        if (!lock.owns_lock()) lock.lock();
        return q.at(index);
    }
    const T &at(const size_t &index) const
    {
        std::unique_lock<std::mutex> lock(q_mut);
        return at(lock, index);
    }

    T &at(std::unique_lock<std::mutex> &lock, const size_t &index)
    {
        if (!lock.owns_lock()) lock.lock();
        return q.at(index);
    }
    T &at(const size_t &index)
    {
        std::unique_lock<std::mutex> lock(q_mut);
        return at(lock, index);
    }

    template <typename... Args>
    requires std::is_constructible_v<T, Args...>
    void emplace_back(Args... args)
    {
        std::unique_lock<std::mutex> lock(q_mut);
        q.emplace_back(args...);
    }

    template <typename... Args>
    requires std::is_constructible_v<T, Args...>
    void emplace_front(Args... args)
    {
        std::unique_lock<std::mutex> lock(q_mut);
        q.emplace_front(args...);
    }

    void push_back(const T &i)
    {
        std::unique_lock<std::mutex> lock(q_mut);
        q.emplace_back(std::move(i));

        vBlocking.notify_one();
    }
    void push_front(const T &i)
    {
        std::unique_lock<std::mutex> lock(q_mut);
        q.emplace_front(std::move(i));

        vBlocking.notify_one();
    }

    std::unique_lock<std::mutex> wait()
    {
        std::unique_lock<std::mutex> ul(mutBlocking);
        while (this->bWait && this->empty()) vBlocking.wait(ul);
        return ul;
    }
    template <unsigned D = 10>
    std::unique_lock<std::mutex> waitTimeout()
    {
        std::unique_lock<std::mutex> ul(mutBlocking);
        vBlocking.wait_for(ul, std::chrono::milliseconds(D));
        return ul;
    }

    std::unique_lock<std::mutex> waitTimeout(unsigned d = 10)
    {
        std::unique_lock<std::mutex> ul(mutBlocking);
        vBlocking.wait_for(ul, std::chrono::milliseconds(d));
        return ul;
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

#undef STORAGE_METHOD_IMPL_BOTH
#undef STORAGE_METHOD_IMPL
#undef STORAGE_METHOD_IMPL_CONST
#undef STORAGE_METHOD_IMPL__