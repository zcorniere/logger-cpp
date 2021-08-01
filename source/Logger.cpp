#include "Logger.hpp"

#include <deque>
#include <exception>
#include <iostream>
#include <optional>
#include <utility>

#define BRACKETS(c, s) "[\e[" << c << "m" << s << "\e[0m] "

Logger::Logger(std::ostream &stream): stream(stream) {}

Logger::~Logger() { this->stop(); }

void Logger::thread_loop()
{
    while (!bExit) {
        try {
            if (qBars.empty())
                qMsg.wait();
            else {
                qMsg.waitTimeout<100>();
                // come up line and clear them to display the messages (seen man console_codes)
                std::unique_lock<std::mutex> ul(mutBars);
                stream << "\033[" << qBars.size() - iNewBars << "F\033[J";
                iNewBars = 0;
            }

            // Flush the messages q
            while (!qMsg.empty()) {
                auto i = qMsg.pop_front();
                if (i) stream << "\33[2K" << *i << "\e[0m" << std::endl;
            }

            // redraw the progress bars
            for (const auto &bar: qBars) bar.update(stream);

            stream.flush();
        } catch (const std::exception &e) {
            std::cerr << "LOGGER ERROR:" << e.what() << std::endl;
        }
    }
}

void Logger::start() { msgT = std::thread(&Logger::thread_loop, this); }

void Logger::stop(bool bFlush)
{
    bExit = true;
    qMsg.setWaitMode(false);

    if (bFlush) this->flush();
    if (msgT.joinable()) { msgT.join(); }
}

void Logger::flush()
{
    std::unique_lock<std::mutex> lBuffers(mutBuffer);

    for (auto &[_, i]: mBuffers) {
        std::string msg(i.str());
        if (!msg.empty()) stream << msg << std::endl;
        i = std::stringstream();
    }
}

void Logger::endl()
{
    std::string msg(this->raw().str());
    qMsg.push_back(msg);
    mBuffers.at(std::this_thread::get_id()) = std::stringstream();
}

std::stringstream &Logger::warn(const std::string &msg)
{
    auto &buf = this->raw();
    buf << BRACKETS(33, "WARN") << BRACKETS(33, msg);
    return buf;
}

std::stringstream &Logger::err(const std::string &msg)
{
    auto &buf = this->raw();
    buf << BRACKETS(31, "ERROR") << BRACKETS(31, msg);
    return buf;
}

std::stringstream &Logger::info(const std::string &msg)
{
    auto &buf = this->raw();
    buf << BRACKETS(0, "INFO") << BRACKETS(0, msg);
    return buf;
}

std::stringstream &Logger::debug(const std::string &msg)
{
    auto &buf = this->raw();
    buf << BRACKETS(36, "DEBUG") << BRACKETS(36, msg);
    return buf;
}

std::stringstream &Logger::msg(const std::string &msg)
{
    auto &buf = this->raw();
    buf << BRACKETS(0, msg);
    return buf;
}

std::stringstream &Logger::raw()
{
    if (!mBuffers.contains(std::this_thread::get_id())) {
        std::unique_lock<std::mutex> lBuffers(mutBuffer);
        mBuffers.insert({std::this_thread::get_id(), std::stringstream()});
    }
    return mBuffers.at(std::this_thread::get_id());
}
