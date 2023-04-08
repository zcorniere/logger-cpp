#include "cpplogger/formatters/ColorFormatter.hpp"
#include "cpplogger/Logger.hpp"
#include "cpplogger/define.hpp"
#include "cpplogger/types/Level.hpp"

#include <iostream>

namespace cpplogger
{

static std::string __Helper(const char *const Format, ...)
{
    va_list ParamInfo;
    va_start(ParamInfo, Format);
    std::string result = Formatf(Format, ParamInfo);
    va_end(ParamInfo);
    return result;
}

const char *ColorFormatter::sPattern = "[%s:%ld][%s%s%s][%s] %s";

ColorFormatter::ColorFormatter() {}

ColorFormatter::~ColorFormatter() {}

std::string ColorFormatter::format(const Message &message)
{
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(message.LogTime.time_since_epoch()) -
              std::chrono::duration_cast<std::chrono::seconds>(message.LogTime.time_since_epoch());

    std::string Time = TimerToString(message.LogTime);
    std::string Color = internal::color(levelColor(message.LogLevel));
    std::string_view LogLevel = to_string(message.LogLevel);

    return __Helper(sPattern, Time.data(), ms.count(), Color.data(), LogLevel.data(), internal::resetSequence,
                    message.CategoryName.data(), message.Message.data());
}

}    // namespace cpplogger
