#include "cpplogger/sinks/StdoutSink.hpp"
#include "cpplogger/define.hpp"
#include "cpplogger/utils/GlobalMutex.hpp"

namespace cpplogger
{

unsigned StdoutSink::initialize_terminal([[maybe_unused]] std::FILE *p_File)
{
#if defined(TERMINAL_TARGET_WINDOWS)
    HANDLE hOut = reinterpret_cast<HANDLE>(::_get_osfhandle(::_fileno(p_File)));
    if (hOut == INVALID_HANDLE_VALUE) return ::GetLastError();
    DWORD dwMode = 0;
    if (!::GetConsoleMode(hOut, &dwMode)) return ::GetLastError();
    dwMode |= DWORD(ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    if (!::SetConsoleMode(hOut, dwMode)) return ::GetLastError();
#elif defined(TERMINAL_TARGET_POSIX)
#endif
    return 0;
}

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

void StdoutSink::SetFormatter(std::unique_ptr<IFormatter> formatter) { std::swap(p_Formatter, formatter); }

}    // namespace cpplogger
