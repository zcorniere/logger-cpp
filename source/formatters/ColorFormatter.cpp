#include "cpplogger/formatters/ColorFormatter.hpp"
#include "cpplogger/define.hpp"
#include "cpplogger/types/Level.hpp"

#include <iostream>

namespace cpplogger
{

ColorFormatter::ColorFormatter() {}

ColorFormatter::~ColorFormatter() {}

std::string ColorFormatter::format(const Message &message)
{
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(message.LogTime.time_since_epoch()) -
              std::chrono::duration_cast<std::chrono::seconds>(message.LogTime.time_since_epoch());

    return fmt::format(fmt::runtime(m_Pattern), fmt::arg("Color", internals::color(levelColor(message.LogLevel))),
                       fmt::arg("Reset", internals::resetSequence), fmt::arg("Category", message.CategoryName),
                       fmt::arg("LogLevel", to_string(message.LogLevel)),
                       fmt::arg("LogLocation", message.LogLocation.function_name()),
                       fmt::arg("LogTime", message.LogTime), fmt::arg("LogTimeMilis", ms.count()),
                       fmt::arg("Message", message.Message));
}

}    // namespace cpplogger
