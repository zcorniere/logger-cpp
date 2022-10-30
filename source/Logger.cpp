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

void Logger::stop(bool bFlush)
{
    this->debug("LOGGER") << "Logger is stopping !";
    deinit();

    context.variable.get_raw().notify_one();
    if (bFlush) this->flush();
}

void Logger::flush()
{
    context.stream << "[LOGGER] Flushing..." << std::endl;
    context.qMsg.lock([&](const auto &mMsg) {
        for (auto &msg: mMsg) context.stream << msg << std::endl;
    });
    context.stream.flush();
}

void Logger::thread_loop(std::stop_token token, Context &context)
{
    while (!token.stop_requested() || context.qMsg.lock([&](const auto &i) { return !i.empty(); })) {
        try {
            {
                auto var = context.variable.lock();
                var.get().wait_for(var.get_lock(), std::chrono::milliseconds(10));
            }

            print(context);
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
