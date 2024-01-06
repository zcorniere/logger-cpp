#pragma once

#include "cpplogger/types/Message.hpp"

namespace cpplogger
{

class DefaultFormatter
{
public:
    static std::string format(const Message &message)
    {
        return std::format("[{0:%F} {0:%T}][{1}][{2}] {3}",
                           std::chrono::floor<std::chrono::milliseconds>(message.LogTime), to_string(message.LogLevel),
                           message.CategoryName, message.Message);
    }
};

}    // namespace cpplogger
