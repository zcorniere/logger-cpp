#include "cpplogger/Logger.hpp"

#include <signal.h>
#include <unordered_map>

static std::unordered_map<std::string, cpplogger::Logger &> s_LoggerStorage;

namespace cpplogger
{

std::string Formatf(const std::string_view format, ...)
{
    va_list ArgList;
    va_start(ArgList, format);

    std::string Format = Formatf(format, ArgList);

    va_end(ArgList);
    return Format;
}

std::string Formatf(const std::string_view format, va_list ArgumentList)
{
    va_list ArgumentList2;
    va_copy(ArgumentList2, ArgumentList);
    size_t RequiredSize = vsnprintf(nullptr, 0, format.data(), ArgumentList2);

    if (RequiredSize < 0) { return std::string("ERROR") + std::string(format); }

    va_end(ArgumentList2);

    RequiredSize += 1;

    std::string Buffer;
    Buffer.resize(RequiredSize);

    va_copy(ArgumentList2, ArgumentList);
    vsnprintf(Buffer.data(), RequiredSize, format.data(), ArgumentList2);
    va_end(ArgumentList2);

    return Buffer;
}

Logger::Logger(const std::string &name): m_Name(name) { internal::LoggerStorage::registerLogger(*this); }

Logger::~Logger() { internal::LoggerStorage::removeLogger(*this); }

void Logger::addSink(std::unique_ptr<ISink> sink, std::unique_ptr<IFormatter> formatter)
{
    if (!sink) throw LoggerError("Null sink");
    if (formatter) sink->SetFormatter(std::move(formatter));
    loggerSinks.emplace_back(std::move(sink));
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
        auto Iter = s_LoggerStorage.find(name);
        if (Iter == s_LoggerStorage.end()) throw Logger::LoggerError("Can't find the logger named " + name);
        return Iter->second;
    }

}    // namespace internal

}    // namespace cpplogger
