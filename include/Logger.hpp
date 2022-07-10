#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>

#include "types/Level.hpp"
#include "types/MessageBuffer.hpp"
#include "types/ProgressBar.hpp"
#include "types/Stream.hpp"
#include "utils/mutex.hpp"

namespace cpplogger
{
class Logger
{
private:
    class Context
    {
    public:
        Context(std::ostream &os): stream(os) {}
        Context(const Context &) = delete;

    public:
        std::ostream &stream;
        std::atomic_bool bExit = false;
        std::atomic_bool bForceExit = false;
        std::atomic<Level> selectedLevel = Level::Debug;

        mutex<std::condition_variable> variable;
        mutex<std::deque<MessageBuffer>> qMsg;
        mutex<std::unordered_map<std::thread::id, MessageBuffer>> mBuffers;
        mutex<std::deque<ProgressBar>> qBars;
    };

public:
    Logger(std::ostream &stream);
    Logger(const Logger &) = delete;
    Logger(const Logger &&) = delete;
    ~Logger();
    void start(Level level = Level::Debug);
    void stop(bool bForce = true, bool bFlush = true);
    void flush();

    void setLevel(Level level);

    template <typename... Args>
    requires std::is_constructible_v<ProgressBar, Args...>
    [[nodiscard]] ProgressBar newProgressBar(Args... args)
    {
        return context.qBars.lock([&](auto &i) {
            i.emplace_back(args...);
            return i.back();
        });
    }

    template <class... DeletedBars>
    void deleteProgressBar(const DeletedBars &...bars)
    {
        context.qBars.lock([... args = std::forward<const DeletedBars>(bars)](auto &i) {
            std::erase_if(i, [... bars = std::forward<const DeletedBars>(args)](const auto &bar) {
                return (((bar == bars) || ...));
            });
        });
    }

    [[nodiscard]] Stream level(Level level, const std::string &msg);
    [[nodiscard]] Stream warn(const std::string &msg = "");
    [[nodiscard]] Stream err(const std::string &msg = "");
    [[nodiscard]] Stream info(const std::string &msg = "");
    [[nodiscard]] Stream debug(const std::string &msg = "");
    [[nodiscard]] Stream trace(const std::string &msg = "");
    [[nodiscard]] Stream msg(const std::string &msg = "");
    void endl();

private:
    void init();
    void deinit();
    [[nodiscard]] MessageBuffer &raw();

    static void thread_loop(Context &context);

private:
    Context context;
    std::jthread msgT;
};

}    // namespace cpplogger

#if defined(LOGGER_EXTERN_DECLARATION_PTR)
extern cpplogger::Logger *logger;
#elif defined(LOGGER_EXTERN_DECLARATION)
extern cpplogger::Logger logger;
#endif
