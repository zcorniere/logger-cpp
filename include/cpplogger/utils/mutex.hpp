#pragma once

#include <concepts>
#include <functional>
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
        inline guard(std::unique_lock<M> lock, T &ref): m_lock(std::move(lock)), ref(ref) {}
        inline guard(M &mutex, T &ref): m_lock(mutex), ref(ref) {}
        guard(const guard &) = delete;
        ~guard() = default;

        inline T &get() noexcept
        {
            if (!owns_lock()) lock();
            return ref;
        }
        inline const T &get() const noexcept
        {
            if (!owns_lock()) lock();
            return ref;
        }

        inline std::unique_lock<M> &get_lock() noexcept { return m_lock; }

        inline void unlock() { m_lock.unlock(); }
        inline void lock() { m_lock.lock(); }
        inline bool try_lock() { return m_lock.try_lock(); }
        inline bool owns_lock() const noexcept { return m_lock.owns_lock(); }

        inline operator bool() const noexcept { return m_lock.owns_lock(); }

    private:
        mutable std::unique_lock<M> m_lock;
        T &ref;
    };

public:
    template <typename... Args>
    inline mutex(Args &...args): data(args...)
    {
    }

    inline guard lock() { return guard(m_mutex, data); }

    template <class Fun>
    requires std::invocable<Fun, T &>
    inline auto lock(Fun &&function)
    {
        auto guard = lock();
        return std::invoke(function, guard.get());
    }

    inline std::optional<guard> try_lock()
    {
        std::unique_lock<M> lock(m_mutex, std::try_to_lock);
        return guard(std::move(lock), data);
    }

    constexpr M &get_mutex() noexcept { return m_mutex; }
    constexpr T &get_raw() noexcept { return data; }

private:
    M m_mutex;
    T data;
};

}    // namespace cpplogger