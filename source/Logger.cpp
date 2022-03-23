#include "Logger.hpp"

#include <deque>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <utility>

#include "Terminal.hpp"

Logger::Logger(std::ostream &stream): stream(stream) { Terminal::setupTerminal(stream); }

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
                stream << LOGGER_ESCAPE_SEQUENCE "[" << barsModifier << "F" LOGGER_ESCAPE_SEQUENCE "[0J";
                barsModifier = 0;
            }

            // Flush the messages queue
            while (!qMsg.empty()) {
                auto msg = qMsg.pop_front();

                if (!msg) throw std::runtime_error("Error getting value");

                if (msg->message) {
                    // If there is a message, print it
                    if (msg->level >= selectedLevel.load())
                        stream << LOGGER_ESCAPE_SEQUENCE "[2K" << msg->message.value() << Terminal::reset << std::endl;
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

#define BRACKETS(COLOR, STRING) "[" << Terminal::color<COLOR> << STRING << Terminal::reset << "] "

Logger::Stream Logger::warn(const std::string_view &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Warn;
    return Stream(*this, buf, msg);
}

Logger::Stream Logger::err(const std::string_view &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Error;
    return Stream(*this, buf, msg);
}

Logger::Stream Logger::info(const std::string_view &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Info;
    return Stream(*this, buf, msg);
}

Logger::Stream Logger::debug(const std::string_view &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Debug;
    return Stream(*this, buf, msg);
}

Logger::Stream Logger::msg(const std::string_view &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Message;
    return Stream(*this, buf, msg);
}

Logger::Stream Logger::trace(const std::string_view &msg)
{
    auto &buf = this->raw();
    buf.level = Logger::Level::Trace;
    return Stream(*this, buf, msg);
}

Logger::MessageBuffer &Logger::raw()
{
    if (!mBuffers.contains(std::this_thread::get_id())) {
        std::stringstream str;
        Terminal::colorize(str);
        std::unique_lock<std::mutex> lBuffers(mutBuffer);
        mBuffers[std::this_thread::get_id()] = {
            .level = Logger::Level::Message,
            .stream = std::move(str),
        };
    }
    return mBuffers.at(std::this_thread::get_id());
}

#undef BRACKETS
