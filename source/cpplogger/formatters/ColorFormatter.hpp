#pragma once

#include "cpplogger/types/Message.hpp"

#if !CPPLOGGER_USE_STD_FORMAT
    #include <fmt/chrono.h>
#endif

namespace cpplogger
{

class ColorFormatter
{
public:
    static std::string format(const Message &message)
    {
#if CPPLOGGER_USE_STD_FORMAT
        return std::format("[{0:%F} {0:%T}][{1:#}][{2}] {3}",
                           std::chrono::floor<std::chrono::milliseconds>(message.LogTime), message.LogLevel,
                           message.CategoryName, message.Message);
#else
        return fmt::format("[{0:%F} {0:%T}][{1:#}][{2}] {3}",
                           std::chrono::floor<std::chrono::milliseconds>(message.LogTime), message.LogLevel,
                           message.CategoryName, message.Message);
#endif
    }
};

}    // namespace cpplogger
