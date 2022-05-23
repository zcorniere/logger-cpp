#include "Size.hpp"

#include "define.hpp"

namespace cpplogger
{

Size Size::get() noexcept
{
#if defined(TERMINAL_TARGET_WINDOWS)
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return {
        .columns = csbi.srWindow.Right - csbi.srWindow.Left + 1,
        .lines = csbi.srWindow.Bottom - csbi.srWindow.Top + 1,
    };
#elif defined(TERMINAL_TARGET_POSIX)
    struct winsize w;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return {
        .columns = w.ws_col,
        .lines = w.ws_row,
    };
#endif
    return {};
}

}    // namespace cpplogger