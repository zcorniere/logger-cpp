#include "cpplogger/Logger.hpp"

#include <unordered_map>

static std::unordered_map<std::string, cpplogger::Logger &> s_LoggerStorage;

namespace cpplogger
{

Logger::Logger(const std::string &name): m_Name(name) { internal::LoggerStorage::registerLogger(*this); }

Logger::~Logger()
{
    internal::LoggerStorage::removeLogger(*this);
    for (auto sink: loggerSinks) { delete sink; }
}

void Logger::addSink(ISink *sink, IFormatter *formatter)
{
    if (!sink) throw std::runtime_error("Null sink");
    if (formatter) sink->SetFormatter(formatter);
    loggerSinks.push_back(sink);
}

void Logger::log(const cpplogger::Message &message)
{
    for (auto sink: loggerSinks) { sink->write(message); }
}

namespace internal
{
    void LoggerStorage::registerLogger(Logger &logger) { s_LoggerStorage.emplace(logger.getName(), logger); }

    void LoggerStorage::removeLogger(Logger &logger) { s_LoggerStorage.erase(logger.getName()); }

    Logger &LoggerStorage::getLogger(const std::string &name) { return s_LoggerStorage.at(name); }

}    // namespace internal

}    // namespace cpplogger
