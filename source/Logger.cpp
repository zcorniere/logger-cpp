#include "cpplogger/Logger.hpp"

#include <deque>
#include <exception>
#include <iostream>
#include <stdexcept>

#ifndef LOGGER_DELAY
    #define LOGGER_DELAY 100ms
#endif

static std::once_flag initInstanceFlag;

extern void backstop();
extern unsigned init_terminal();

namespace cpplogger
{

Logger::Logger(std::ostream &stream): context(stream) { std::call_once(initInstanceFlag, init_terminal); }

Logger::~Logger() { this->stop(false); }

void Logger::start(Level level)
{
    init();
    context.selectedLevel = level;
    msgT = std::jthread(Logger::thread_loop, std::ref(context));
    this->debug("LOGGER") << "Logger started !";
}

void Logger::stop(bool bForce, bool bFlush)
{
    this->debug("LOGGER") << "Logger is stopping !";
    deinit();

    context.variable.get_raw().notify_one();
    context.bExit = true;
    if (bForce) context.bForceExit = true;
    if (bForce && bFlush) this->flush();
}

void Logger::flush()
{
    context.stream << "[LOGGER] Flushing..." << std::endl;
    context.qMsg.lock([&](const auto &mMsg) {
        for (auto &msg: mMsg) context.stream << msg << std::endl;
    });
    context.stream.flush();
}

void Logger::print() { Logger::print(context); }

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

void Logger::thread_loop(Context &context)
{
    while (!context.bForceExit) {
        try {
            {
                using namespace std::literals;

                auto var = context.variable.lock();
                var.get().wait_for(var.get_lock(), LOGGER_DELAY);
            }

            print(context);

            if (context.bExit && context.qMsg.lock([&](const auto &i) { return i.empty(); })) { return; }
        } catch (const std::exception &e) {
            std::cerr << "LOGGER ERROR: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error in the Logger thread. Will exit now..." << std::endl;
            return;
        }
    }
}

void Logger::print(Logger::Context &context)
{
    static int barsModifier = 0;

    std::stringstream bufferStream;
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

    context.updateQueue.lock([&](auto &bars) {
        if (bars.empty()) return;

        // redraw the progress bars
        for (const auto &bar: bars) {
            bar->update(bufferStream);
            barsModifier += 1;
        }
    });
    context.stream << bufferStream.str();
    context.stream.flush();
}

}    // namespace cpplogger
