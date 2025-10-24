#include "cpplogger/Logger.hpp"

#if CPPLOGGER_NO_EXCEPTIONS
    #include <cassert>
#endif    // !CPPLOGGER_NO_EXCEPTIONS

#include <mutex>
#include <shared_mutex>
#include <unordered_map>

static std::unordered_map<std::string, cpplogger::Logger &> s_LoggerStorage;
static std::shared_mutex s_LoggerStorageMutex;

namespace cpplogger
{
Logger::Logger(const std::string &name): Name(name) { internal::LoggerStorage::registerLogger(*this); }

Logger::~Logger() { internal::LoggerStorage::removeLogger(*this); }

ISink *Logger::addSink(ISink *const sink)
{
    loggerSinks.push_back(sink);
    return sink;
}

bool Logger::removeSink(ISink *const sink)
{
    const auto iter = std::find(loggerSinks.begin(), loggerSinks.end(), sink);
    if (iter != loggerSinks.end()) {
        loggerSinks.erase(iter);
        return true;
    }
    return false;
}

namespace internal
{
    void LoggerStorage::registerLogger(Logger &logger)
    {
        std::unique_lock lock(s_LoggerStorageMutex);
        s_LoggerStorage.emplace(logger.getName(), logger);
    }

    void LoggerStorage::removeLogger(Logger &logger)
    {
        std::unique_lock lock(s_LoggerStorageMutex);
        s_LoggerStorage.erase(logger.getName());
    }

    Logger &LoggerStorage::getLogger(const std::string &name)
    {
        std::shared_lock Lock(s_LoggerStorageMutex);

        const std::unordered_map<std::string, cpplogger::Logger &>::const_iterator iter = s_LoggerStorage.find(name);
        if (iter != s_LoggerStorage.end()) [[likely]] { return iter->second; }

#if !CPPLOGGER_NO_EXCEPTIONS
    #if CPPLOGGER_USE_STD_FORMAT
        throw LoggerError(std::format("{:s} logger not registered !", name));
    #else
        throw LoggerError(fmt::format("{:s} logger not registered !", name));
    #endif
#else
        assert("Logger is not registered");
        std::abort();
#endif    // CPPLOGGER_NO_EXCEPTIONS
    }

}    // namespace internal

}    // namespace cpplogger
