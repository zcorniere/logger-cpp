#include "cpplogger/formatters/DefaultFormatter.hpp"
#include "cpplogger/Logger.hpp"
#include "cpplogger/define.hpp"
#include "cpplogger/types/Level.hpp"

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

const char *DefaultFormatter::sPattern = "[%s:%ld][%s][%s] %s";

DefaultFormatter::DefaultFormatter() {}

DefaultFormatter::~DefaultFormatter() {}

std::string DefaultFormatter::format(const Message &message)
{
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(message.LogTime.time_since_epoch()) -
              std::chrono::duration_cast<std::chrono::seconds>(message.LogTime.time_since_epoch());
    std::string Time = TimerToString(message.LogTime);
    std::string_view LogLevel = to_string(message.LogLevel);

    return __Helper(sPattern, Time.data(), ms.count(), LogLevel.data(), message.CategoryName.data(),
                    message.Message.data());
}

}    // namespace cpplogger
