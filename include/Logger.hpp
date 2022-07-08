#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>

#include "types/Level.hpp"
#include "types/MessageBuffer.hpp"
#include "types/ProgressBar.hpp"
#include "types/Stream.hpp"
#include "utils/concepts.hpp"
#include "utils/mutex.hpp"
#include "utils/sequences.hpp"
#include "utils/source_location.hpp"

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
        std::atomic<Level> selectedLevel = Level::Debug;

        mutex<std::condition_variable> variable;
        mutex<std::deque<MessageBuffer>> qMsg;
        mutex<std::unordered_map<std::thread::id, MessageBuffer>> mBuffers;
        mutex<std::deque<std::pair<bool, ProgressBar>>> qBars;
    };

public:
    Logger(std::ostream &stream);
    Logger(const Logger &) = delete;
    Logger(const Logger &&) = delete;
    ~Logger();
    void start(Level level = Level::Debug);
    void stop(bool bFlush = true);
    void flush();

    void setLevel(Level level);

    template <typename... Args>
    requires std::is_constructible_v<ProgressBar, Args...>
    [[nodiscard]] ProgressBar newProgressBar(Args... args)
    {
        auto bar = context.qBars.lock();
        bar.get().emplace_back(std::make_pair(false, ProgressBar(args...)));
        return bar.get().back().second;
    }

    template <class... deletedBars>
    void deleteProgressBar(const deletedBars &...bar)
    {
        for (auto &[needDeletion, i]: context.qBars.lock().get()) {
            if (((i == bar) || ...)) { needDeletion = true; }
        }
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
