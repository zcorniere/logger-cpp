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
    enum class Level { Trace = -1, Debug = 0, Info = 1, Warn = 2, Error = 3, Message = 4 };

    class Stream
    {
    public:
        constexpr Stream(Logger &log, std::stringstream &ss): s(ss), logger(log) {}
        ~Stream() { logger.endl(); }

        template <typename T>
        constexpr Stream &operator<<(const T &u)
        {
            s << u;
            return *this;
        }

    private:
        std::stringstream &s;
        Logger &logger;
    };

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
    [[nodiscard]] ProgressBar newProgressBar(Args... args)
    {
        qBars.emplace_back(std::make_pair(false, ProgressBar(args...)));
        return qBars.back().second;
    }

    template <class... deletedBars>
    void deleteProgressBar(const deletedBars &...bar)
    {
        for (auto &[needDeletion, i]: qBars) {
            if (((i == bar) || ...)) { needDeletion = true; }
        }
    }

    [[nodiscard]] Stream warn(const std::string_view &msg = "WARNING");
    [[nodiscard]] Stream err(const std::string_view &msg = "ERROR");
    [[nodiscard]] Stream info(const std::string_view &msg = "INFO");
    [[nodiscard]] Stream debug(const std::string_view &msg = "DEBUG");
    [[nodiscard]] Stream trace(const std::string_view &msg = "DEBUG");
    [[nodiscard]] Stream msg(const std::string_view &msg = "MESSAGE");

private:
    void endl();
    [[nodiscard]] Logger::MessageBuffer &raw();
    void thread_loop();

private:
    std::ostream &stream;
    std::mutex mutBuffer;
    std::atomic_bool bExit = false;
    std::thread msgT;
    std::atomic<Level> selectedLevel = Level::Debug;
    ThreadSafeStorage<Message> qMsg;
    std::unordered_map<std::thread::id, MessageBuffer> mBuffers;

    // Progress Bars
    ThreadSafeStorage<std::pair<bool, ProgressBar>> qBars;
    std::atomic<int16_t> iNewBars = 0;
};

#if defined(LOGGER_EXTERN_DECLARATION_PTR)
    #define LOGGER_ACCESS ->
extern Logger *logger;

#elif defined(LOGGER_EXTERN_DECLARATION)
    #define LOGGER_ACCESS .
extern Logger logger;

#endif

#ifndef LOGGER_NO_MACROS

    #define S1(x) #x
    #define S2(x) S1(x)
    #define LOCATION __FILE__ ":" S2(__LINE__)

    #ifdef LOGGER_ACCESS
        #define LOGGER_WARN logger LOGGER_ACCESS warn(LOCATION)
        #define LOGGER_ERR logger LOGGER_ACCESS err(LOCATION)
        #define LOGGER_INFO logger LOGGER_ACCESS info(LOCATION)
        #define LOGGER_DEBUG logger LOGGER_ACCESS debug(LOCATION)

    #else

        #define LOGGER_WARN(logger) logger.warn(LOCATION)
        #define LOGGER_ERR(logger) logger.err(LOCATION)
        #define LOGGER_INFO(logger) logger.info(LOCATION)
        #define LOGGER_DEBUG(logger) logger.debug(LOCATION)

    #endif    // LOGGER_NO_MACROS

#endif
