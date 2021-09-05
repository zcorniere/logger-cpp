#pragma once

#include <atomic>
#include <iterator>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>

#include "ProgressBar.hpp"
#include "ThreadedQ.hpp"

template <typename... Args>
concept isProgressBar = requires
{
    std::is_constructible_v<ProgressBar, Args...>;
};

class Logger
{
public:
    enum class Level : uint8_t { Debug = 0, Info = 1, Warn = 2, Error = 4, Message = 5 };

    struct MessageBuffer {
        Level level = Level::Message;
        std::stringstream stream{};
    };

    struct Message {
        Level level = Level::Message;
        std::optional<std::string> message{};
    };

public:
    Logger(std::ostream &stream);
    ~Logger();
    void start(Level level = Level::Debug);
    void stop(bool bFlush = true);
    void flush();

    Level getLevel() const;
    void setLevel(Level level);

    template <isProgressBar... Args>
    [[nodiscard]] ProgressBar &newProgressBar(Args... args)
    {
        std::unique_lock<std::mutex> lBuffers(mutBars);
        qBars.emplace_back(args...);
        iNewBars += 1;
        qMsg.notify();
        return qBars.back();
    }

    template <class... deletedBars>
    void deleteProgressBar(const deletedBars &...bar)
    {
        std::unique_lock<std::mutex> lBuffers(mutBars);
        const auto e = std::remove_if(qBars.begin(), qBars.end(), [&](const auto &i) { return ((i == bar) || ...); });
        iNewBars -= static_cast<int16_t>(std::distance(e, qBars.end()));
        qBars.erase(e, qBars.end());
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
    ThreadedQ<Message> qMsg;
    std::unordered_map<std::thread::id, MessageBuffer> mBuffers;

    // Progress Bars
    ThreadedQ<ProgressBar> qBars;
    std::mutex mutBars;
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
