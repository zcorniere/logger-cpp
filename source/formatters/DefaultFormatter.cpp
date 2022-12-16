#include "cpplogger/formatters/DefaultFormatter.hpp"
#include "cpplogger/define.hpp"
#include "cpplogger/types/Level.hpp"

namespace cpplogger
{

DefaultFormatter::DefaultFormatter() {}

DefaultFormatter::~DefaultFormatter() {}

std::string DefaultFormatter::format(const Message &message)
{
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(message.LogTime.time_since_epoch()) -
              std::chrono::duration_cast<std::chrono::seconds>(message.LogTime.time_since_epoch());

    return fmt::format(fmt::runtime(m_Pattern), fmt::arg("Category", message.CategoryName),
                       fmt::arg("LogLevel", to_string(message.LogLevel)),
                       fmt::arg("LogLocation", message.LogLocation.function_name()),
                       fmt::arg("LogTime", message.LogTime), fmt::arg("LogTimeMilis", ms.count()),
                       fmt::arg("Message", message.Message));
}

}    // namespace cpplogger