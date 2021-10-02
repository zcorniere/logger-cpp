#include "Logger.hpp"

#include <deque>
#include <exception>
#include <iostream>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <utility>

#define ESCAPE_SEQUENCE "\u001b"

#define COLOR_CODE(COLOR) ESCAPE_SEQUENCE "[" #COLOR "m"

#define BRACKETS(COLOR, STRING) "[" COLOR_CODE(COLOR) << STRING << COLOR_CODE(0) "] "

Logger::Logger(std::ostream &stream): stream(stream) {}

Logger::~Logger() { this->stop(); }

void Logger::thread_loop()
{
    while (!bExit || !qMsg.empty()) {
        try {
            qMsg.waitTimeout<100>();

            int barsModifier = std::accumulate(qBars.begin(), qBars.end(), 0, [](const int prev, const auto &ser) {
                if (ser.first == ProgressBar::Ok || ser.first == ProgressBar::Delete)
                    return prev + 1;
                else
                    return prev;
            });

            if (barsModifier)
                // come up some line and clear them to display the messages (see man console_codes)
                stream << ESCAPE_SEQUENCE "[" << barsModifier << "F" ESCAPE_SEQUENCE "[J";

            // Flush the messages queue
            while (!qMsg.empty()) {
                auto msg = qMsg.pop_front();

                if (!msg) throw std::runtime_error("Error getting value");

                if (msg->message) {
                    // If there is a message, print it
                    if (msg->level >= selectedLevel.load())
                        stream << ESCAPE_SEQUENCE "[2K" << *(msg->message) << ESCAPE_SEQUENCE "[0m" << std::endl;
                } else {
                    // If not, set the level
                    selectedLevel = msg->level;
                }
            }

            qBars.erase([](const auto &i) { return i.first == ProgressBar::Delete; });

            // redraw the progress bars
            for (auto &[e, bar]: qBars) {
                if (e == ProgressBar::New) e = ProgressBar::Ok;
                bar.update(stream);
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
    msgT = std::jthread(&Logger::thread_loop, this);
}

void Logger::stop(bool bFlush)
{
    bExit = true;
    qMsg.setWaitMode(false);

    if (bFlush) this->flush();
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

std::stringstream &Logger::warn(const std::string &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Warn;
    buf.stream << BRACKETS(33, "WARN") BRACKETS(33, msg);
    return buf.stream;
}

std::stringstream &Logger::err(const std::string &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Error;
    buf.stream << BRACKETS(31, "ERROR") BRACKETS(31, msg);
    return buf.stream;
}

std::stringstream &Logger::info(const std::string &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Info;
    buf.stream << BRACKETS(36, "INFO") BRACKETS(36, msg);
    return buf.stream;
}

std::stringstream &Logger::debug(const std::string &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Debug;
    buf.stream << BRACKETS(35, "DEBUG") BRACKETS(35, msg);
    return buf.stream;
}

std::stringstream &Logger::msg(const std::string &msg)
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