#pragma once

#include "cpplogger/define.hpp"
#include "cpplogger/sinks/ISink.hpp"
#include "cpplogger/types/Message.hpp"
#include "cpplogger/internal/StringLiteral.hpp"

#include <format>
#include <memory>
#include <string>
#include <vector>

namespace cpplogger
{

class LoggerError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class Logger
{
public:
    Logger(const std::string &name);
    ~Logger();

    template <template <class> class T, Formatter TForm, typename... ArgsTypes>
        requires std::is_constructible_v<T<TForm>, ArgsTypes...> && std::derived_from<T<TForm>, ISink>
    inline void addSink(ArgsTypes &&...args)
    {
        loggerSinks.emplace_back(new T<TForm>(std::forward<ArgsTypes>(args)...));
    }

    inline void log(const Message &message)
    {
        for (auto &sink: loggerSinks) { sink->write(message); }
    }

    constexpr const std::string &getName() const { return m_Name; }

private:
    const std::string m_Name;
    std::vector<ISink *> loggerSinks;
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
    template <Level Loglevel, typename... Args>
    static inline void log(const std::string_view &patern, const Args &...args)
    {
        if constexpr (Loglevel <= CompileTimeVerbosity) {
            internal::LoggerStorage::getLogger(Logger.value)
                .log(Message{
                    .LogLevel = Loglevel,
                    .CategoryName = Name.value,
                    .Message = std::vformat(patern, std::make_format_args(args...)),
                });
        }
    }

    template <typename... Args>
    static inline void log(Level level, const std::string_view &patern, const Args &...args)
    {
        if (level <= CompileTimeVerbosity) {
            internal::LoggerStorage::getLogger(Logger.value)
                .log(Message{
                    .LogLevel = level,
                    .CategoryName = Name.value,
                    .Message = std::vformat(patern, std::make_format_args(args...)),
                });
        }
    }
};

}    // namespace cpplogger

#define DECLARE_LOGGER_CATEGORY(Logger, Name, Verbosity) \
    using Name = ::cpplogger::LoggerScope<#Logger, #Name, ::cpplogger::Level::Verbosity>;

#define LOG_V(Name, Verbosity, Pattern, ...) Name::log(Verbosity, Pattern __VA_OPT__(, )  __VA_ARGS__)
#define LOG(Name, Verbosity, Pattern, ...) Name::log<::cpplogger::Level::Verbosity>(Pattern __VA_OPT__(, ) __VA_ARGS__)