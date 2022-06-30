#pragma once

#include <mutex>
#include <optional>

namespace cpplogger
{

template <typename T, typename M = std::mutex>
class mutex
{
public:
    class guard
    {
    public:
        guard(std::unique_lock<M> lock, T &ref): m_lock(std::move(lock)), ref(ref) {}
        guard(M &mutex, T &ref): m_lock(mutex), ref(ref) {}

        T &get() noexcept
        {
            if (!owns_lock()) lock();
            return ref;
        }
        const T &get() const noexcept
        {
            if (!owns_lock()) lock();
            return ref;
        }

        void unlock() { m_lock.unlock(); }
        void lock() { m_lock.lock(); }
        bool try_lock() { return m_lock.try_lock(); }
        bool owns_lock() const noexcept { return m_lock.owns_lock(); }

        operator bool() const noexcept { return m_lock.owns_lock(); }

    private:
        mutable std::unique_lock<M> m_lock;
        T &ref;
    };

public:
    template <typename... Args>
    mutex(Args &...args): data(args...)
    {
    }

    guard lock() { return guard(m_mutex, data); }

    std::optional<guard> try_lock()
    {
        std::unique_lock<M> lock(m_mutex, std::try_to_lock);
        return guard(std::move(lock), data);
    }

    M &get_mutex() noexcept { return m_mutex; }

private:
    M m_mutex;
    T data;
};

}    // namespace cpplogger