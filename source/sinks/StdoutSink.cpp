#include "cpplogger/sinks/StdoutSink.hpp"
#include "cpplogger/define.hpp"
#include "cpplogger/utils/GlobalMutex.hpp"

namespace cpplogger
{

StdoutSink::StdoutSink(std::FILE *file): p_File(file), r_Mutex(internal::ConsoleMutex::mutex()) {}

void StdoutSink::write(const Message &message)
{
    if (!p_Formatter) { throw std::runtime_error("Missing formatter in StdoutSink !"); }
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(message.LogTime.time_since_epoch()) -
              std::chrono::duration_cast<std::chrono::seconds>(message.LogTime.time_since_epoch());

    std::string formatter_string = p_Formatter->format(message) + "\n";

    std::unique_lock lock(r_Mutex);
    std::fwrite(formatter_string.data(), sizeof(char), formatter_string.size(), p_File);
    std::fflush(p_File);
}

void StdoutSink::flush()
{
    std::unique_lock lock(r_Mutex);
    std::fflush(p_File);
}

void StdoutSink::SetFormatter(IFormatter *formatter)
{
    if (p_Formatter) { delete p_Formatter; }
    p_Formatter = formatter;
}

}    // namespace cpplogger