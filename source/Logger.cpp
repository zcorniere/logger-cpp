#include "Logger.hpp"

#include <deque>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <utility>

static std::once_flag initInstanceFlag;

static unsigned init()
{
#if defined(TERMINAL_TARGET_WINDOWS)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return GetLastError();
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return GetLastError();
    dwMode |= DWORD(ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    if (!SetConsoleMode(hOut, dwMode)) return GetLastError();
#elif defined(TERMINAL_TARGET_POSIX)
#endif
    return 0;
}

namespace cpplogger
{

Logger::Logger(std::ostream &stream): stream(stream) { std::call_once(initInstanceFlag, init); }

Logger::~Logger() { this->stop(); }

void Logger::thread_loop()
{
    int barsModifier = 0;
    while (!bExit || !qMsg.empty()) {
        try {
            if (qBars.empty())
                qMsg.wait();
            else
                qMsg.waitTimeout<100>();

            while (barsModifier > 0) {
                stream << moveUp(1) << clearLine();
                barsModifier -= 1;
            }

            // Flush the messages queue
            while (!qMsg.empty()) {
                auto msg = qMsg.pop_front();

                if (!msg) throw std::runtime_error("Error getting value");

                if (msg->message) {
                    // If there is a message, print it
                    if (msg->level >= selectedLevel) stream << clearLine() << msg->message.value() << reset() << "\n";
                } else {
                    // If not, set the level
                    selectedLevel = msg->level;
                }
            }

            qBars.erase([](const auto &i) { return i.first; });

            // redraw the progress bars
            for (auto &[_, bar]: qBars) {
                bar.update(stream);
                barsModifier++;
            }
            stream.flush();
        } catch (const std::exception &e) {
            std::cerr << "LOGGER ERROR: " << e.what() << std::endl;
        }
    }
}

void Logger::start(Logger::Level level)
{
    selectedLevel = level;
    msgT = std::thread(&Logger::thread_loop, this);
}

void Logger::stop(bool bFlush)
{
    bExit = true;
    qMsg.setWaitMode(false);

    if (msgT.joinable()) msgT.join();
    if (bFlush) this->flush();
}

void Logger::flush()
{
    std::unique_lock<std::mutex> lBuffers(mutBuffer);

    for (auto &msg: qMsg)
        if (msg.message) stream << msg.message.value() << std::endl;
    for (auto &[_, i]: mBuffers) {
        std::string msg(i.stream.str());
        if (!msg.empty()) stream << msg << std::endl;
        i.stream = std::stringstream();
    }
}

void Logger::setLevel(Level level)
{
    qMsg.push_back({
        .level = level,
        .message = std::nullopt,
    });
}

void Logger::endl()
{
    auto &raw = this->raw();
    qMsg.push_back({
        .level = raw.level,
        .message = raw.stream.str(),
    });
    mBuffers.erase(std::this_thread::get_id());
}

Logger::Stream Logger::level(Logger::Level level, const std::string_view &msg)
{
    switch (level) {
        case Level::Trace: return trace(msg);
        case Level::Debug: return debug(msg);
        case Level::Info: return info(msg);
        case Level::Warn: return warn(msg);
        case Level::Error: return err(msg);
        case Level::Message: return this->msg(msg);
        default: throw std::runtime_error("Invalid level value");
    }
}

#define LOGGER_FUNC(LVL)     \
    auto &buf = this->raw(); \
    buf.level = LVL;         \
    return Stream(*this, buf, msg);

Logger::Stream Logger::warn(const std::string_view &msg) { LOGGER_FUNC(Logger::Level::Warn); }

Logger::Stream Logger::err(const std::string_view &msg) { LOGGER_FUNC(Logger::Level::Error); }

Logger::Stream Logger::info(const std::string_view &msg) { LOGGER_FUNC(Logger::Level::Info); }

Logger::Stream Logger::debug(const std::string_view &msg) { LOGGER_FUNC(Logger::Level::Debug); }

Logger::Stream Logger::msg(const std::string_view &msg) { LOGGER_FUNC(Logger::Level::Message); }

Logger::Stream Logger::trace(const std::string_view &msg) { LOGGER_FUNC(Logger::Level::Trace); }

Logger::MessageBuffer &Logger::raw()
{
    if (!mBuffers.contains(std::this_thread::get_id())) {
        std::unique_lock<std::mutex> lBuffers(mutBuffer);
        mBuffers[std::this_thread::get_id()] = {};
    }
    return mBuffers.at(std::this_thread::get_id());
}

}    // namespace cpplogger
