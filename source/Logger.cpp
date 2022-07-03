#include "Logger.hpp"

#include <algorithm>
#include <cassert>
#include <deque>
#include <exception>
#include <iostream>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <utility>

static std::once_flag initInstanceFlag;

extern void backstop();
extern unsigned init_terminal();

namespace cpplogger
{

Logger::Logger(std::ostream &stream): stream(stream) { std::call_once(initInstanceFlag, init_terminal); }

Logger::~Logger() { this->stop(); }

std::pair<std::size_t, std::optional<Logger::Message>> Logger::getMessage()
{
    auto lMsg = qMsg.lock();
    if (lMsg.get().empty()) return {0, std::nullopt};

    auto msg = std::move(lMsg.get().front());
    lMsg.get().pop_front();
    return {lMsg.get().size(), std::move(msg)};
}

void Logger::thread_loop()
{
    using namespace std::literals;

    int barsModifier = 0;
    while (!bExit) {
        try {
            std::this_thread::sleep_for(100ms);

            while (barsModifier > 0) {
                stream << moveUp(1) << clearLine();
                barsModifier -= 1;
            }

            // Flush the messages queue
            while (1) {
                const auto &[size, msg] = getMessage();

                if (size == 0 || !msg.has_value()) break;

                if (msg->message) {
                    // If there is a message, print it
                    if (msg->level >= selectedLevel)
                        stream << clearLine() << msg->message.value() << reset() << std::endl;
                } else {
                    // If not, set the level
                    selectedLevel = msg->level;
                }
            }

            {
                auto lBar = qBars.lock();
                auto &mBar = lBar.get();
                if (!mBar.empty()) {
                    const auto &remove_index =
                        std::remove_if(mBar.begin(), mBar.end(), [](const auto &i) { return i.first; });
                    mBar.erase(remove_index);

                    // redraw the progress bars
                    for (auto &[_, bar]: mBar) {
                        bar.update(stream);
                        barsModifier++;
                    }
                }
            }

            stream.flush();
        } catch (const std::exception &e) {
            std::cerr << "LOGGER ERROR: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown erro in the Logger thread. Will exit now..." << std::endl;
            return;
        }
    }
}

void Logger::start(Logger::Level level)
{
    init();
    selectedLevel = level;
    msgT = std::thread(&Logger::thread_loop, this);
}

void Logger::stop(bool bFlush)
{
    deinit();

    bExit = true;

    if (msgT.joinable()) msgT.join();
    if (bFlush) this->flush();
}

void Logger::flush()
{
    std::unique_lock<std::mutex> lBuffers(mutBuffer);

    for (auto &msg: qMsg.lock().get())
        if (msg.message) stream << msg.message.value() << std::endl;
    for (auto &[_, i]: mBuffers) {
        std::string msg(i.stream.str());
        if (!msg.empty()) stream << msg << std::endl;
        i.stream = std::stringstream();
    }
}

void Logger::setLevel(Level level)
{
    auto lock = qMsg.lock();
    lock.get().push_back({
        .level = level,
        .message = std::nullopt,
    });
}

void Logger::endl()
{
    auto &raw = this->raw();
    auto lock = qMsg.lock();

    lock.get().push_back({
        .level = raw.level,
        .message = raw.stream.str(),
    });
    mBuffers.at(std::this_thread::get_id()) = {};
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
    std::unique_lock<std::mutex> lBuffers(mutBuffer);
    return mBuffers[std::this_thread::get_id()];
}

}    // namespace cpplogger
