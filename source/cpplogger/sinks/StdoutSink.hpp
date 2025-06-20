#pragma once

#include "cpplogger/define.hpp"
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
    StdoutSink(std::FILE *const file): p_File(file), r_Mutex(internal::ConsoleMutex::mutex()) { initialize_terminal(); }
    virtual ~StdoutSink() {}

    virtual void write(const Message &message) override
    {
        const std::string formatter_string = T::format(message) + "\n";

        std::unique_lock lock(r_Mutex);
        std::fwrite(formatter_string.data(), sizeof(char), formatter_string.size(), p_File);
    }
    virtual void flush() override
    {
        std::unique_lock lock(r_Mutex);
        std::fflush(p_File);
    }

private:
    static void initialize_terminal()
    {
#if defined(TERMINAL_TARGET_WINDOWS)
        auto HandleStdOutput = [](DWORD Input) {
            HANDLE hOut = GetStdHandle(Input);
            if (hOut == INVALID_HANDLE_VALUE) { return; }
            DWORD dwMode = 0;
            if (!::GetConsoleMode(hOut, &dwMode)) { return; }
            dwMode |= DWORD(ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            if (!::SetConsoleMode(hOut, dwMode)) { return; }
        };
        HandleStdOutput(STD_OUTPUT_HANDLE);
        HandleStdOutput(STD_ERROR_HANDLE);

#elif defined(TERMINAL_TARGET_POSIX)
        // nothing to do ANSI codes are enabled by default on POSIX terminals
#else
    #error "Unsupported platform"
#endif
    }

private:
    std::FILE *const p_File;
    std::mutex &r_Mutex;
};

}    // namespace cpplogger
