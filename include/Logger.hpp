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
concept HasIterator = std::ranges::range<T> && requires(const T a)
{
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
    class Message
    {
    public:
        Message(const Level level): level(level) {}
        Message(const MessageBuffer &buffer): level(buffer.level), message(buffer.stream.str()) {}

    public:
        Level level = Level::Message;
        std::optional<std::string> message = std::nullopt;
    };

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
        mutex<std::deque<Message>> qMsg;
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

    static void thread_loop(Context &context);

private:
    Context context;
    std::jthread msgT;
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
