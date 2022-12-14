#include <iostream>

#include "cpplogger/define.hpp"

unsigned init_terminal()
{
    static bool initialized = false;
    if (initialized == true) { return 0; }
#if defined(TERMINAL_TARGET_WINDOWS)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return GetLastError();
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return GetLastError();
    dwMode |= DWORD(ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    if (!SetConsoleMode(hOut, dwMode)) return GetLastError();
#elif defined(TERMINAL_TARGET_POSIX)
#endif
    initialized = true;
    return 0;
}