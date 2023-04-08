#pragma once

#include "cpplogger/types/Message.hpp"

namespace cpplogger
{

class IFormatter
{
public:
    virtual ~IFormatter() {}

    virtual std::string format(const Message &message) = 0;

protected:
    std::string TimerToString(const std::chrono::system_clock::time_point &LogTime)
    {
        std::string Buffer;
        time_t timer = std::chrono::system_clock::to_time_t(LogTime);
        struct tm *tm_info = localtime(&timer);

        Buffer.resize(26);
        strftime(Buffer.data(), 26, "%Y-%m-%d %H:%M:%S", tm_info);
        return Buffer;
    }
};

}    // namespace cpplogger
