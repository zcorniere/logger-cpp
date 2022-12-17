#pragma once

#include "cpplogger/define.hpp"
#include "cpplogger/sinks/ISink.hpp"
#include "cpplogger/types/Message.hpp"
#include "cpplogger/utils/StringLiteral.hpp"

#include <memory>
#include <string>
#include <vector>

namespace cpplogger
{

class Logger
{
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
    template <typename... Args>
    static void log(Level level, const std::string &patern, Args... args)
    {
        if (level >= CompileTimeVerbosity) {
            internal::LoggerStorage::getLogger(Logger.value)
                .log(Message{
                    .LogLevel = level,
                    .CategoryName = Name.value,
                    .Message = fmt::format(fmt::runtime(patern), args...),
                });
        }
    }
};

}    // namespace cpplogger

#define DELARE_LOGGER_CATEGORY(Logger, Name, Verbosity) \
    using Name = ::cpplogger::LoggerScope<#Logger, #Name, ::cpplogger::Level::Verbosity>;

#define LOG(Name, Verbosity, Pattern, ...) Name::log(::cpplogger::Level::Verbosity, Pattern __VA_OPT__(, ) __VA_ARGS__)
