#pragma once

#include "cpplogger/internal/GlobalMutex.hpp"
#include "cpplogger/sinks/ISink.hpp"

#include <cstdio>
#include <mutex>

namespace cpplogger
{

template <Formatter T>
class StdoutSink : public TSink<T>
{
public:
    StdoutSink(std::FILE *const file): p_File(file), r_Mutex(internal::ConsoleMutex::mutex()) {}
    virtual ~StdoutSink() {}

    virtual void write(const Message &message) override
    {
        std::string formatter_string = T::format(message) + "\n";

        std::unique_lock lock(r_Mutex);
        std::fwrite(formatter_string.data(), sizeof(char), formatter_string.size(), p_File);
        std::fflush(p_File);
    }
    virtual void flush() override
    {
        std::unique_lock lock(r_Mutex);
        std::fflush(p_File);
    }

private:
    static unsigned initialize_terminal(std::FILE *const p_File)
    {
#if defined(TERMINAL_TARGET_WINDOWS)
        HANDLE hOut = reinterpret_cast<HANDLE>(::_get_osfhandle(::_fileno(p_File)));
        if (hOut == INVALID_HANDLE_VALUE) return ::GetLastError();
        DWORD dwMode = 0;
        if (!::GetConsoleMode(hOut, &dwMode)) return ::GetLastError();
        dwMode |= DWORD(ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        if (!::SetConsoleMode(hOut, dwMode)) return ::GetLastError();
#elif defined(TERMINAL_TARGET_POSIX)
        (void)p_File;
#endif
        return 0;
    }

private:
    std::FILE *const p_File;
    std::mutex &r_Mutex;
};

}    // namespace cpplogger
