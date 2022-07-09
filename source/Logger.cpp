#include "Logger.hpp"

#include <deque>
#include <exception>
#include <iostream>
#include <stdexcept>

static std::once_flag initInstanceFlag;

extern void backstop();
extern unsigned init_terminal();

namespace cpplogger
{

Logger::Logger(std::ostream &stream): context(stream) { std::call_once(initInstanceFlag, init_terminal); }

Logger::~Logger() { this->stop(false); }

void Logger::thread_loop(Context &context)
{
    int barsModifier = 0;

    while (!context.bExit) {
        std::stringstream bufferStream;
        try {
            {
                using namespace std::literals;

                auto var = context.variable.lock();
                var.get().wait_for(var.get_lock(), 100ms);
            }

            if (barsModifier > 0) {
                bufferStream << moveUp(barsModifier) << clearAfterCursor();
                barsModifier = 0;
            }

            // Flush the messages queue
            context.qMsg.lock([&](auto &i) {
                for (; !i.empty(); i.pop_front()) {
                    const auto &msg = i.front();
                    if (!msg.content) {
                        context.selectedLevel = msg.level;
                    } else if (msg.level >= context.selectedLevel) {
                        bufferStream << msg << std::endl;
                    }
                }
            });

            context.qBars.lock([&](auto &bars) {
                if (bars.empty()) return;

                // redraw the progress bars
                for (const auto &bar: bars) {
                    bar.update(bufferStream);
                    barsModifier += 1;
                }
            });
            context.stream << bufferStream.str();
            context.stream.flush();
        } catch (const std::exception &e) {
            std::cerr << "LOGGER ERROR: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error in the Logger thread. Will exit now..." << std::endl;
            return;
        }
    }
}

void Logger::start(Level level)
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
    context.qMsg.lock([&](const auto &mMsg) {
        for (auto &msg: mMsg) context.stream << msg;
    });

    context.mBuffers.lock([&](auto &mBuffer) {
        for (auto &[_, msg]: mBuffer) context.stream << msg;
    });
}

void Logger::setLevel(Level level)
{
    context.qMsg.lock([level](auto &i) {
        i.push_back({
            .level = level,
        });
    });
}

void Logger::endl()
{
    auto &buf = this->raw();

    context.qMsg.lock([&buf](auto &i) { i.push_back(buf); });
    buf = {};
}

Stream Logger::level(Level level, const std::string &msg)
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

Stream Logger::warn(const std::string &msg) { LOGGER_FUNC(Level::Warn); }

Stream Logger::err(const std::string &msg) { LOGGER_FUNC(Level::Error); }

Stream Logger::info(const std::string &msg) { LOGGER_FUNC(Level::Info); }

Stream Logger::debug(const std::string &msg) { LOGGER_FUNC(Level::Debug); }

Stream Logger::msg(const std::string &msg) { LOGGER_FUNC(Level::Message); }

Stream Logger::trace(const std::string &msg) { LOGGER_FUNC(Level::Trace); }

MessageBuffer &Logger::raw() { return context.mBuffers.lock().get()[std::this_thread::get_id()]; }

}    // namespace cpplogger
