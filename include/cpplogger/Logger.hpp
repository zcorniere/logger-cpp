#pragma once

#include "cpplogger/define.hpp"
#include "cpplogger/internal/StringLiteral.hpp"
#include "cpplogger/sinks/ISink.hpp"
#include "cpplogger/types/Message.hpp"

#include <memory>
#include <string>
#include <vector>

#define __CPPLOGGER_FORMAT_ATTRIBUTE(Format) __attribute__((format(printf, Format, Format + 1)))

namespace cpplogger
{

std::string Formatf(const std::string_view format, ...);
std::string Formatf(const std::string_view format, va_list ArgumentList);

class Logger
{
public:
    class LoggerError : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

public:
    Logger(const std::string &name);
    ~Logger();

    void addSink(std::unique_ptr<ISink> sink, std::unique_ptr<IFormatter> formatter = nullptr);
    void log(const Message &message);

    const std::string &getName() const { return m_Name; }

private:
    std::string m_Name;
    std::vector<std::unique_ptr<ISink>> loggerSinks;
};

namespace internal
{
    class LoggerStorage
    {
    public:
        static void registerLogger(Logger &logger);
        static void removeLogger(Logger &logger);
        static Logger &getLogger(const std::string &name);
    };
}    // namespace internal

template <internal::StringLiteral Logger, internal::StringLiteral Name, Level CompileTimeVerbosity>
class LoggerScope
{
public:
    template <Level Loglevel>
    __CPPLOGGER_FORMAT_ATTRIBUTE(1)
    static void log(const char *patern, ...)
    {
        if constexpr (Loglevel >= CompileTimeVerbosity) {
            va_list ParamInfo;
            va_start(ParamInfo, patern);

            internal::LoggerStorage::getLogger(Logger.value)
                .log(Message{
                    .LogLevel = Loglevel,
                    .CategoryName = Name.value,
                    .Message = Formatf(patern, ParamInfo),
                });
            va_end(ParamInfo);
        }
    }

    __CPPLOGGER_FORMAT_ATTRIBUTE(2)
    static void log(Level level, const char *patern, ...)
    {
        if (level >= CompileTimeVerbosity) {
            va_list ParamInfo;
            va_start(ParamInfo, patern);

            internal::LoggerStorage::getLogger(Logger.value)
                .log(Message{
                    .LogLevel = level,
                    .CategoryName = Name.value,
                    .Message = Formatf(patern, ParamInfo),
                });

            va_end(ParamInfo);
        }
    }
};

}    // namespace cpplogger

#define DECLARE_LOGGER_CATEGORY(Logger, Name, Verbosity) \
    using Name = ::cpplogger::LoggerScope<#Logger, #Name, ::cpplogger::Level::Verbosity>;

#define LOG_V(Name, Verbosity, Pattern, ...) Name::log(Verbosity, Pattern __VA_OPT__(, ) __VA_ARGS__)
#define LOG(Name, Verbosity, Pattern, ...) Name::log<::cpplogger::Level::Verbosity>(Pattern __VA_OPT__(, ) __VA_ARGS__)
