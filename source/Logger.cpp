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

Logger::Logger(std::ostream &stream): context(stream) { std::call_once(initInstanceFlag, init_terminal); }

Logger::~Logger() { this->stop(false); }

void Logger::thread_loop(Context &context)
{
    using namespace std::literals;

    int barsModifier = 0;
    while (!context.bExit) {
        std::stringstream barStringLogger;
        try {
            {
                auto var = context.variable.lock();
                var.get().wait_for(var.get_lock(), 100ms);
            }

            while (barsModifier > 0) {
                barStringLogger << moveUp(1) << clearLine();
                barsModifier--;
            }

            // Flush the messages queue
            context.qMsg.lock([&](auto &i) -> void {
                for (; !i.empty(); i.pop_front()) {
                    const auto &msg = i.front();
                    if (msg.message) {
                        if (msg.level >= context.selectedLevel)
                            barStringLogger << clearLine() << msg.message.value() << reset() << std::endl;
                    } else {
                        context.selectedLevel = msg.level;
                    }
                }
            });

            context.qBars.lock([&](auto &bars) -> void {
                if (bars.empty()) return;
                std::erase_if(bars, [](const auto &i) { return i.first; });

                // redraw the progress bars
                for (const auto &[_, bar]: bars) {
                    bar.update(barStringLogger);
                    barsModifier++;
                }
            });
            context.stream << barStringLogger.str();
        } catch (const std::exception &e) {
            std::cerr << "LOGGER ERROR: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error in the Logger thread. Will exit now..." << std::endl;
            return;
        }
    }
}

void Logger::start(Logger::Level level)
{
    init();
    context.selectedLevel = level;
    msgT = std::jthread(Logger::thread_loop, std::ref(context));
}

void Logger::stop(bool bFlush)
{
    deinit();

    context.variable.get_raw().notify_one();
    context.bExit = true;
    if (bFlush) this->flush();
}

void Logger::flush()
{
    context.qMsg.lock([&](auto &mMsg) {
        for (auto &msg: mMsg)
            if (msg.message) context.stream << msg.message.value() << std::endl;
    });

    context.mBuffers.lock([&](auto &mBuffer) -> void {
        for (auto &[_, i]: mBuffer) {
            std::string msg(i.stream.str());
            if (!msg.empty()) context.stream << msg << std::endl;
            i.stream = std::stringstream();
        }
    });
}

void Logger::setLevel(Level level)
{
    context.qMsg.lock([level](auto &i) -> void { i.emplace_back(level); });
}

void Logger::endl()
{
    auto &buf = this->raw();

    context.qMsg.lock([&buf](auto &i) -> void { i.emplace_back(buf); });
    context.mBuffers.lock([](auto &i) -> void { i.at(std::this_thread::get_id()) = {}; });
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

Logger::MessageBuffer &Logger::raw() { return context.mBuffers.lock().get()[std::this_thread::get_id()]; }

}    // namespace cpplogger
