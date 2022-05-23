#pragma once

#include <atomic>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <stdint.h>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>

#include "ProgressBar.hpp"
#include "ThreadSafeStorage.hpp"
#include "utils.hpp"

template <typename T>
concept Printable = requires(T a)
{
    std::cout << a;
};

template <typename T>
concept HasIterator = requires(const T a)
{
    {
        a.begin()
        } -> std::same_as<typename T::const_iterator>;
    {
        a.end()
        } -> std::same_as<typename T::const_iterator>;
    {
        a.size()
        } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept PrintableIterator =
    HasIterator<T> && !Printable<T> && Printable<typename std::iterator_traits<typename T::const_iterator>::value_type>;

namespace cpplogger
{
class Logger
{
public:
    enum class Level {
        Trace = -1,
        Debug = 0,
        Info = 1,
        Warn = 2,
        Error = 3,
        Message = 4,
    };

protected:
    struct MessageBuffer {
        Level level = Level::Message;
        std::stringstream stream;
        constexpr const std::string_view levelStr() const noexcept
        {
            switch (level) {
                case Level::Trace: return "TRACE";
                case Level::Debug: return "DEBUG";
                case Level::Info: return "INFO";
                case Level::Warn: return "WARN";
                case Level::Error: return "ERROR";
                case Level::Message: return "MESSAGE";
                default: return "UNKNOWN";
            }
        }
        constexpr Color levelColor() const noexcept
        {
            switch (level) {
                case Level::Trace: return Color::Green;
                case Level::Debug: return Color::Magenta;
                case Level::Info: return Color::Cyan;
                case Level::Warn: return Color::Yellow;
                case Level::Error: return Color::Red;
                case Level::Message: return Color::White;
                default: return Color::White;
            }
        }
    };

    class Stream
    {
    public:
        inline explicit Stream(Logger &log, MessageBuffer &buffer, const std::string_view &message)
            : buffer(buffer), logger(log)
        {
            buffer.stream << "[" << color(buffer.levelColor());
            if (std::uncaught_exceptions() > 0) buffer.stream << "THROW/";
            buffer.stream << buffer.levelStr() << reset() << "]";
            buffer.stream << "[" << color(buffer.levelColor()) << message << reset() << "] ";
        }
        ~Stream() { logger.endl(); }

        inline Stream &operator<<(const Printable auto &object)
        {
            buffer.stream << object;
            return *this;
        }

        inline Stream &operator<<(const PrintableIterator auto &container)
        {
            auto size = container.size() - 1;
            buffer.stream << "[";
            for (const auto &i: container) {
                buffer.stream << '\"';
                (*this) << i;
                buffer.stream << '\"';
                if (size-- > 0) buffer.stream << ", ";
            }
            buffer.stream << "]";
            return *this;
        }

    private:
        MessageBuffer &buffer;
        Logger &logger;
    };

private:
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

    Level getLevel() const noexcept { return selectedLevel; };
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

    [[nodiscard]] Stream level(Level level, const std::string_view &msg);
    [[nodiscard]] Stream warn(const std::string_view &msg = "WARNING");
    [[nodiscard]] Stream err(const std::string_view &msg = "ERROR");
    [[nodiscard]] Stream info(const std::string_view &msg = "INFO");
    [[nodiscard]] Stream debug(const std::string_view &msg = "DEBUG");
    [[nodiscard]] Stream trace(const std::string_view &msg = "TRACE");
    [[nodiscard]] Stream msg(const std::string_view &msg = "MESSAGE");
    void endl();

private:
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

}    // namespace cpplogger

#if defined(LOGGER_EXTERN_DECLARATION_PTR)
    #define LOGGER_ACCESS ->
extern cpplogger::Logger *logger;

#elif defined(LOGGER_EXTERN_DECLARATION)
    #define LOGGER_ACCESS .
extern cpplogger::Logger logger;

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
