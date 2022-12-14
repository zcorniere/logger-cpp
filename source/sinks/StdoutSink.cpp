#include "cpplogger/sinks/StdoutSink.hpp"
#include "cpplogger/define.hpp"
#include "cpplogger/utils/GlobalMutex.hpp"

#include <fmt/chrono.h>

namespace cpplogger
{

StdoutSink::StdoutSink(std::FILE *file): p_File(file), r_Mutex(internal::ConsoleMutex::mutex()) {}

void StdoutSink::write(const Message &message)
{
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(message.LogTime.time_since_epoch()) -
              std::chrono::duration_cast<std::chrono::seconds>(message.LogTime.time_since_epoch());

    std::string formatter_string =
        fmt::format("[{:%F} {:%T}:{:3}][{}][{}] {}", message.LogTime, message.LogTime, ms.count(),
                    to_string(message.LogLevel), message.CategoryName, message.Message);

    std::unique_lock lock(r_Mutex);
    std::fwrite(formatter_string.data(), sizeof(char), formatter_string.size(), p_File);
    std::fflush(p_File);
}

void StdoutSink::flush()
{
    std::unique_lock lock(r_Mutex);
    std::fflush(p_File);
}

}    // namespace cpplogger