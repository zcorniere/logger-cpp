#include "Logger.hpp"

#include <deque>
#include <exception>
#include <iostream>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <utility>

#include "macros.h"

Logger::Logger(std::ostream &stream): stream(stream) {}

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

            if (barsModifier) {
                // come up some line and clear them to display the messages (see man console_codes)
                stream << ESCAPE_SEQUENCE "[" << barsModifier << "F" ANSI_SEQUENCE(, J);
                barsModifier = 0;
            }

            // Flush the messages queue
            while (!qMsg.empty()) {
                auto msg = qMsg.pop_front();

                if (!msg) throw std::runtime_error("Error getting value");

                if (msg->message) {
                    // If there is a message, print it
                    if (msg->level >= selectedLevel.load())
                        stream << ANSI_SEQUENCE(2, K) << *(msg->message) << RESET_SEQUENCE << std::endl;
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
            std::cerr << "LOGGER ERROR:" << e.what() << std::endl;
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

    if (bFlush) this->flush();
    if (msgT.joinable()) msgT.join();
}

void Logger::flush()
{
    std::unique_lock<std::mutex> lBuffers(mutBuffer);

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
    mBuffers.at(std::this_thread::get_id()) = {
        .level = Level::Message,
        .stream = std::stringstream(),
    };
}

std::stringstream &Logger::warn(const std::string_view &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Warn;
    buf.stream << BRACKETS(YELLOW, "WARN") BRACKETS(YELLOW, msg);
    return buf.stream;
}

std::stringstream &Logger::err(const std::string_view &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Error;
    buf.stream << BRACKETS(RED, "ERROR") BRACKETS(RED, msg);
    return buf.stream;
}

std::stringstream &Logger::info(const std::string_view &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Info;
    buf.stream << BRACKETS(CYAN, "INFO") BRACKETS(CYAN, msg);
    return buf.stream;
}

std::stringstream &Logger::debug(const std::string_view &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Debug;
    buf.stream << BRACKETS(MAGENTA, "DEBUG") BRACKETS(MAGENTA, msg);
    return buf.stream;
}

std::stringstream &Logger::msg(const std::string_view &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Message;
    buf.stream << BRACKETS(0, msg);
    return buf.stream;
}

Logger::MessageBuffer &Logger::raw()
{
    if (!mBuffers.contains(std::this_thread::get_id())) {
        std::unique_lock<std::mutex> lBuffers(mutBuffer);
        mBuffers[std::this_thread::get_id()] = {
            .level = Logger::Level::Message,
            .stream = std::stringstream(),
        };
    }
    return mBuffers.at(std::this_thread::get_id());
}

#undef COLOR_CODE
#undef BRACKETS
#undef ESCAPE_SEQUENCE