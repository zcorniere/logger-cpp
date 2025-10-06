#pragma once

#include "cpploggerBuildConfig.hpp"

#include "cpplogger/internal/StringLiteral.hpp"
#include "cpplogger/sinks/ISink.hpp"
#include "cpplogger/types/Message.hpp"

#include <format>
#include <string>
#include <vector>

namespace cpplogger
{

#if !CPPLOGGER_NO_EXCEPTIONS
class LoggerError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};
#endif    // CPPLOGGER_NO_EXCEPTIONS

class Logger
{
public:
    CPPLOGGER_API Logger(const std::string &name);
    CPPLOGGER_API ~Logger();

    template <template <class> class T, Formatter TForm, typename... ArgsTypes>
        requires std::is_constructible_v<T<TForm>, ArgsTypes...> && std::derived_from<T<TForm>, ISink>
    inline T<TForm> *addSink(ArgsTypes &&...args)
    {
        T<TForm> *const NewSink = new T<TForm>(std::forward<ArgsTypes>(args)...);
        loggerSinks.push_back(NewSink);
        return NewSink;
    }

    inline void log(const Message &message)
    {
        for (ISink *const sink: loggerSinks) { sink->write(message); }
    }

    constexpr const std::string &getName() const { return Name; }

private:
    const std::string Name;
    std::vector<ISink *> loggerSinks;
};

namespace internal
{
    class LoggerStorage
    {
    public:
        CPPLOGGER_API static void registerLogger(Logger &logger);
        CPPLOGGER_API static void removeLogger(Logger &logger);
        CPPLOGGER_API static Logger &getLogger(const std::string &name);
    };
}    // namespace internal

template <internal::StringLiteral Logger, internal::StringLiteral Name, Level CompileTimeVerbosity>
class LoggerScope
{
public:
    template <Level Loglevel, typename... ArgsType>
    static inline void log(const std::format_string<ArgsType...> &patern, ArgsType &&...args)
    {
        if constexpr (Loglevel <= CompileTimeVerbosity) {
            internal::LoggerStorage::getLogger(Logger.value)
                .log(Message{
                    .LogLevel = Loglevel,
                    .CategoryName = Name.value,
                    .Message = std::format(patern, std::forward<ArgsType>(args)...),
                });
        } else {
            (void)patern;
            (void(args), ...);
        }
    }

    template <typename... ArgsType>
    static inline void log(const Level level, const std::format_string<ArgsType...> &patern, ArgsType &&...args)
    {
        if (level <= CompileTimeVerbosity) {
            internal::LoggerStorage::getLogger(Logger.value)
                .log(Message{
                    .LogLevel = level,
                    .CategoryName = Name.value,
                    .Message = std::format(patern, std::forward<ArgsType>(args)...),
                });
        }
    }
};

}    // namespace cpplogger

#define DECLARE_LOGGER_CATEGORY(Logger, Name, Verbosity) \
    using Name = ::cpplogger::LoggerScope<#Logger, #Name, ::cpplogger::Level::Verbosity>;

#define LOG_V(Name, Verbosity, Pattern, ...) Name::log(Verbosity, Pattern __VA_OPT__(, ) __VA_ARGS__)
#define LOG(Name, Verbosity, Pattern, ...) Name::log<::cpplogger::Level::Verbosity>(Pattern __VA_OPT__(, ) __VA_ARGS__)
