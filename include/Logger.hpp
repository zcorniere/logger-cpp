#pragma once

#include <atomic>
#include <iterator>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>

#include "ProgressBar.hpp"
#include "ThreadedQ.hpp"

class Logger
{
public:
    Logger(std::ostream &stream);
    ~Logger();
    void start();
    void stop(bool bFlush = true);
    void flush();

    template <class... Args>
    [[nodiscard]] ProgressBar &newProgressBar(Args &&...args)
    {
        std::unique_lock<std::mutex> lBuffers(mutBars);
        qBars.emplace_back(args...);
        iNewBars += 1;
        qMsg.notify();
        return qBars.back();
    }

    template <class... deletedBars>
    void deleteProgressBar(const deletedBars &...bar)
    {
        std::unique_lock<std::mutex> lBuffers(mutBars);
        auto e = std::remove_if(qBars.begin(), qBars.end(), [&](const auto &i) { return (((i == bar) || ...)); });
        iNewBars -= std::distance(e, qBars.end());
        qBars.erase(e, qBars.end());
    }

    void endl();
    std::stringstream &warn(const std::string &msg = "WARNING");
    std::stringstream &err(const std::string &msg = "ERROR");
    std::stringstream &info(const std::string &msg = "INFO");
    std::stringstream &debug(const std::string &msg = "DEBUG");
    std::stringstream &msg(const std::string &msg = "MESSAGE");
    std::stringstream &raw();

private:
    void thread_loop();

private:
    std::ostream &stream;
    std::mutex mutBuffer;
    std::atomic_bool bExit = false;
    std::jthread msgT;
    ThreadedQ<std::string> qMsg;
    std::unordered_map<std::thread::id, std::stringstream> mBuffers;

    // Progress Bars
    ThreadedQ<ProgressBar> qBars;
    std::mutex mutBars;
    std::atomic<int16_t> iNewBars = 0;
};

#if defined(LOGGER_EXTERN_DECLARATION_PTR)

#define LOGGER_ACCESS ->
extern Logger *logger;

#elif defined(LOGGER_EXTERN_DECLARATION)

#define LOGGER_ACCESS .
extern Logger logger;

#endif

#ifdef LOGGER_ACCESS
#ifndef LOGGER_NO_MACROS

#define S1(x) #x
#define S2(x) S1(x)
#define LOCATION __FILE__ ":" S2(__LINE__)

#define LOGGER_WARN logger LOGGER_ACCESS warn(LOCATION)
#define LOGGER_ERR logger LOGGER_ACCESS err(LOCATION)
#define LOGGER_INFO logger LOGGER_ACCESS info(LOCATION)
#define LOGGER_DEBUG logger LOGGER_ACCESS debug(LOCATION)
#define LOGGER_ENDL logger LOGGER_ACCESS endl();

#endif    // LOGGER_NO_MACROS
#endif
