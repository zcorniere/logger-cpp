#pragma once

#include "cpplogger/define.hpp"
#include "cpplogger/types/Level.hpp"

namespace cpplogger
{

class ColorFormatter
{
public:
    static std::string format(const Message &message)
    {
        return std::format("[{0:%F} {0:%T}][{1}{2}{3}][{4}] {5}",
                           std::chrono::floor<std::chrono::milliseconds>(message.LogTime),
                           internal::color(levelColor(message.LogLevel)), to_string(message.LogLevel),
                           internal::resetSequence, message.CategoryName, message.Message);
    }
};

}    // namespace cpplogger
