#include "cpplogger/Logger.hpp"

#include <cassert>
#include <unordered_map>

static std::unordered_map<std::string, cpplogger::Logger &> s_LoggerStorage;

namespace cpplogger
{

Logger::Logger(const std::string &name): m_Name(name) { internal::LoggerStorage::registerLogger(*this); }

Logger::~Logger()
{
    for (ISink *Sink: loggerSinks) { delete Sink; }
    internal::LoggerStorage::removeLogger(*this);
}

void Logger::log(const cpplogger::Message &message)
{
    for (auto &sink: loggerSinks) { sink->write(message); }
}

namespace internal
{
    void LoggerStorage::registerLogger(Logger &logger) { s_LoggerStorage.emplace(logger.getName(), logger); }

    void LoggerStorage::removeLogger(Logger &logger) { s_LoggerStorage.erase(logger.getName()); }

    Logger &LoggerStorage::getLogger(const std::string &name)
    {
        auto iter = s_LoggerStorage.begin();
        if (iter != s_LoggerStorage.end()) { return iter->second; }

        throw LoggerError(fmt::format("{:s} logger not registered !", name));
    }

}    // namespace internal

}    // namespace cpplogger
