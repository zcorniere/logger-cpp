#pragma once

#include <atomic>
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

#include "ProgressBar.hpp"
#include "utils/mutex.hpp"
#include "utils/sequences.hpp"
#include "utils/source_location.hpp"

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
        Stream(const Stream &) = delete;
        Stream(const Stream &&) = delete;
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
    Logger(const Logger &) = delete;
    Logger(const Logger &&) = delete;
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
        auto bar = qBars.lock();
        bar.get().emplace_back(std::make_pair(false, ProgressBar(args...)));
        return bar.get().back().second;
    }

    template <class... deletedBars>
    void deleteProgressBar(const deletedBars &...bar)
    {
        for (auto &[needDeletion, i]: qBars.lock().get()) {
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
    void init();
    void deinit();
    [[nodiscard]] Logger::MessageBuffer &raw();
    void thread_loop();

    std::pair<std::size_t, std::optional<Message>> getMessage();

private:
    std::ostream &stream;
    std::atomic_bool bExit = false;

    std::thread msgT;
    std::atomic<Level> selectedLevel = Level::Debug;
    mutex<std::deque<Message>> qMsg;

    std::mutex mutBuffer;
    std::unordered_map<std::thread::id, MessageBuffer> mBuffers;

    // Progress Bars
    mutex<std::deque<std::pair<bool, ProgressBar>>> qBars;
};

}    // namespace cpplogger

#if defined(LOGGER_EXTERN_DECLARATION_PTR)
    #define LOGGER_ACCESS ->
extern cpplogger::Logger *logger;

#elif defined(LOGGER_EXTERN_DECLARATION)
    #define LOGGER_ACCESS .
extern cpplogger::Logger logger;

#endif

#ifdef LOGGER_ACCESS

    #define LOGGER_WARN(location) logger LOGGER_ACCESS warn(file_position())
    #define LOGGER_ERR(location) logger LOGGER_ACCESS err(file_position())
    #define LOGGER_INFO(location) logger LOGGER_ACCESS info(file_position())
    #define LOGGER_DEBUG(location) logger LOGGER_ACCESS debug(file_position())

#endif
