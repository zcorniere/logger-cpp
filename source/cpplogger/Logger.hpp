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

    /// Create a new sink and add it to the logger
    /// @note not recommended to use if compiling as a shared library and defining global new/delete operator
    template <template <class> class T, Formatter TForm, typename... ArgsTypes>
        requires std::is_constructible_v<T<TForm>, ArgsTypes...> && std::derived_from<T<TForm>, ISink>
    T<TForm> *addSink(ArgsTypes &&...args)
    {
        T<TForm> *const NewSink = new T<TForm>(std::forward<ArgsTypes>(args)...);
        loggerSinks.push_back(NewSink);
        return NewSink;
    }

    /// Add a sink to the logger.
    /// @note the logger **DOES NOT** take ownership of the sink
    ISink *addSink(ISink *const sink)
    {
        loggerSinks.push_back(sink);
        return sink;
    }

    /// Remove a sink from the logger
    bool removeSink(ISink *const sink)
    {
        const auto iter = std::find(loggerSinks.begin(), loggerSinks.end(), sink);
        if (iter != loggerSinks.end()) {
            loggerSinks.erase(iter);
            return true;
        }
        return false;
    }

    /// Log a message to all the sinks of the logger
    inline void log(const Message &message)
    {
        for (ISink *const sink: loggerSinks) { sink->write(message); }
    }

    constexpr const std::string &getName() const { return Name; }
    std::vector<ISink *> &getSinks() { return loggerSinks; }

private:
    const std::string Name;
    std::vector<ISink *> loggerSinks;
};

namespace internal
{
    class LoggerStorage
    {
    public:
        static void registerLogger(Logger &logger);
        static void removeLogger(Logger &logger);
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

/// Declare a new logger category, with a given name and verbosity level
#define DECLARE_LOGGER_CATEGORY(Logger, Name, Verbosity) \
    using Name = ::cpplogger::LoggerScope<#Logger, #Name, ::cpplogger::Level::Verbosity>;

/// Log a message to a logger category with a given verbosity level
#define LOG_V(Name, Verbosity, Pattern, ...) Name::log(Verbosity, Pattern __VA_OPT__(, ) __VA_ARGS__)
#define LOG(Name, Verbosity, Pattern, ...) Name::log<::cpplogger::Level::Verbosity>(Pattern __VA_OPT__(, ) __VA_ARGS__)
