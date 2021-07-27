#pragma once

#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>

#include "ProgressBar.hpp"
#include "ThreadedQ.hpp"

#define S1(x) #x
#define S2(x) S1(x)
#define LOCATION __FILE__ ":" S2(__LINE__)
#define BRACKETS(c, s) "[\e[" << c << "m" << s << "\e[0m] "

class Logger
{
public:
    Logger(std::ostream &stream);
    ~Logger();
    void start();
    void stop();
    void flush();

    std::shared_ptr<ProgressBar> newProgressBar(const std::string &message = "", uint64_t uMax = 100);
    void deleteProgressBar(const std::shared_ptr<ProgressBar> &bar);

    void endl();
    std::stringstream &warn(const std::string &msg = "WARNING");
    std::stringstream &err(const std::string &msg = "ERROR");
    std::stringstream &info(const std::string &msg = "INFO");
    std::stringstream &debug(const std::string &msg = "DEBUG");
    std::stringstream &msg(const std::string &msg = "MESSAGE");
    std::stringstream &raw();

private:
    void thread_loop();

    std::ostream &stream;
    std::mutex mutBuffer;
    std::atomic_bool bExit = false;
    std::thread msgT;
    ThreadedQ<std::string> qMsg;
    std::deque<std::shared_ptr<ProgressBar>> qBars;
    std::atomic_int newBars = 0;
    std::unordered_map<std::thread::id, std::stringstream> mBuffers;
};

extern Logger *logger;

#define LOGGER_WARN logger->warn(LOCATION)
#define LOGGER_ERR logger->err(LOCATION)
#define LOGGER_INFO logger->info(LOCATION)
#define LOGGER_DEBUG logger->debug(LOCATION)
#define LOGGER_ENDL logger->endl();
