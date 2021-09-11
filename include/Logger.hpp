#pragma once

#include <atomic>
#include <mutex>
#include <optional>
#include <sstream>
#include <stdint.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>

#include "ProgressBar.hpp"
#include "ThreadSafeStorage.hpp"

class Logger
{
public:
    enum class Level : uint8_t { Debug = 0, Info = 1, Warn = 2, Error = 4, Message = 5 };

private:
    struct MessageBuffer {
        Level level = Level::Message;
        std::stringstream stream{};
    };

    struct Message {
        Level level = Level::Message;
        std::optional<std::string> message = std::nullopt;
    };

public:
    Logger(std::ostream &stream);
    ~Logger();
    void start(Level level = Level::Debug);
    void stop(bool bFlush = true);
    void flush();

    inline Level getLevel() const noexcept { return selectedLevel; };
    void setLevel(Level level);

    template <typename... Args>
    requires std::is_constructible_v<ProgressBar, Args...>
    [[nodiscard]] ProgressBar &newProgressBar(Args... args)
    {
        qBars.emplace_back(std::make_pair(ProgressBar::New, ProgressBar(args...)));
        return qBars.back().second;
    }

    template <class... deletedBars>
    void deleteProgressBar(const deletedBars &...bar)
    {
        for (auto &[e, i]: qBars) {
            if (((i == bar) || ...)) { e = ProgressBar::Delete; }
        }
    }

    void endl();
    [[nodiscard]] std::stringstream &warn(const std::string &msg = "WARNING");
    [[nodiscard]] std::stringstream &err(const std::string &msg = "ERROR");
    [[nodiscard]] std::stringstream &info(const std::string &msg = "INFO");
    [[nodiscard]] std::stringstream &debug(const std::string &msg = "DEBUG");
    [[nodiscard]] std::stringstream &msg(const std::string &msg = "MESSAGE");
    [[nodiscard]] Logger::MessageBuffer &raw();

private:
    void thread_loop();

private:
    std::ostream &stream;
    std::mutex mutBuffer;
    std::atomic_bool bExit = false;
    std::jthread msgT;
    std::atomic<Level> selectedLevel = Level::Debug;
    ThreadSafeStorage<Message> qMsg;
    std::unordered_map<std::thread::id, MessageBuffer> mBuffers;

    // Progress Bars
    ThreadSafeStorage<std::pair<ProgressBar::Status, ProgressBar>> qBars;
    std::atomic<int16_t> iNewBars = 0;
};

#if defined(LOGGER_EXTERN_DECLARATION_PTR)

#define LOGGER_ACCESS ->
extern Logger *logger;

#elif defined(LOGGER_EXTERN_DECLARATION)

#define LOGGER_ACCESS .
extern Logger logger;

#endif

#ifdef LOGGER_ACCESS
#ifndef LOGGER_NO_MACROS

#define S1(x) #x
#define S2(x) S1(x)
#define LOCATION __FILE__ ":" S2(__LINE__)

#define LOGGER_WARN logger LOGGER_ACCESS warn(LOCATION)
#define LOGGER_ERR logger LOGGER_ACCESS err(LOCATION)
#define LOGGER_INFO logger LOGGER_ACCESS info(LOCATION)
#define LOGGER_DEBUG logger LOGGER_ACCESS debug(LOCATION)
#define LOGGER_ENDL logger LOGGER_ACCESS endl();

#endif    // LOGGER_NO_MACROS
#endif
