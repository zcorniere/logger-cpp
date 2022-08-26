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
        std::atomic_bool bExit = false;
        std::atomic_bool bForceExit = false;
        std::atomic<Level> selectedLevel = Level::Debug;

        mutex<std::condition_variable> variable;
        mutex<std::deque<MessageBuffer>> qMsg;
        mutex<std::unordered_map<std::thread::id, MessageBuffer>> mBuffers;
        mutex<std::deque<std::shared_ptr<IUpdate>>> updateQueue;
    };

public:
    Logger(std::ostream &stream);
    Logger(const Logger &) = delete;
    Logger(const Logger &&) = delete;
    ~Logger();
    void start(Level level = Level::Debug);
    void stop(bool bForce = true, bool bFlush = true);
    void flush();
    void print();

    void setLevel(Level level);

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
    static void print(Context &context);

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
