#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>

#include "interface/IUpdate.hpp"
#include "types/Level.hpp"
#include "types/MessageBuffer.hpp"
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
        std::atomic<Level> selectedLevel = Level::Debug;

        mutex<std::condition_variable> variable;
        mutex<std::deque<MessageBuffer>> qMsg;
        mutex<std::deque<std::shared_ptr<IUpdate>>> updateQueue;
    };

public:
    Logger(std::ostream &stream);
    Logger(const Logger &) = delete;
    Logger(const Logger &&) = delete;
    ~Logger();
    void start(Level level = Level::Debug);
    void stop(bool bFlush = true);
    void flush();

    inline void print() { Logger::print(context); }

    inline void setLevel(Level level)
    {
        context.qMsg.lock([level](auto &i) {
            i.push_back({
                .level = level,
            });
        });
    }

    template <typename T, typename... Args>
    requires std::is_base_of_v<IUpdate, T> && std::is_constructible_v<T, Args...>
    [[nodiscard]] std::shared_ptr<T> add(Args... args)
    {
        auto newBar = std::make_shared<T>(args...);
        context.updateQueue.lock([newBar](auto &i) { i.push_back(newBar); });
        return newBar;
    }

    template <class... T>
    requires(std::is_base_of_v<IUpdate, T> &&...) void remove(const std::shared_ptr<T> &...bars)
    {
        context.updateQueue.lock([... args = std::forward<const std::shared_ptr<T>>(bars)](auto &i) {
            std::erase_if(i, [... bars = std::forward<const std::shared_ptr<T>>(args)](const auto &bar) {
                return (((bar == bars) || ...));
            });
        });
    }

#define LOGGER_METHOD(Name, Level) \
    [[nodiscard]] inline Stream Name(const std::string &msg = "") { return Stream(*this, Level, msg); }

    LOGGER_METHOD(warn, Level::Warn)
    LOGGER_METHOD(err, Level::Error)
    LOGGER_METHOD(info, Level::Info)
    LOGGER_METHOD(debug, Level::Debug)
    LOGGER_METHOD(msg, Level::Message)
    LOGGER_METHOD(trace, Level::Trace)

#undef LOGGER_METHOD

    [[nodiscard]] inline Stream level(Level level, const std::string &message)
    {
        switch (level) {
            case Level::Trace: return trace(message);
            case Level::Debug: return debug(message);
            case Level::Info: return info(message);
            case Level::Warn: return warn(message);
            case Level::Error: return err(message);
            case Level::Message: return msg(message);
            // Should no be executed
            default: return msg(message);
        }
    }

private:
    inline void endl(MessageBuffer buffer)
    {
        context.qMsg.lock([buffer = std::move(buffer)](auto &i) { i.push_back(std::move(buffer)); });
    }

    void init();
    void deinit();

    static void thread_loop(std::stop_token token, Context &context);
    static void print(Context &context);

private:
    Context context;
    std::jthread msgT;

    friend cpplogger::Stream;
};

}    // namespace cpplogger

#if defined(LOGGER_EXTERN_DECLARATION_PTR)
extern cpplogger::Logger *logger;
#elif defined(LOGGER_EXTERN_DECLARATION)
extern cpplogger::Logger logger;
#endif
