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
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(message.LogTime.time_since_epoch()) -
                  std::chrono::duration_cast<std::chrono::seconds>(message.LogTime.time_since_epoch());

        return fmt::format(
            "[{LogTime:%F} {LogTime:%T}:{LogTimeMilis:03}][{Color}{LogLevel}{Reset}][{Category}] {Message}",
            fmt::arg("Color", internal::color(levelColor(message.LogLevel))),
            fmt::arg("Reset", internal::resetSequence), fmt::arg("Category", message.CategoryName),
            fmt::arg("LogLevel", to_string(message.LogLevel)),
            fmt::arg("LogLocation", message.LogLocation.function_name()), fmt::arg("LogTime", message.LogTime),
            fmt::arg("LogTimeMilis", ms.count()), fmt::arg("Message", message.Message));
    }
};

}    // namespace cpplogger
