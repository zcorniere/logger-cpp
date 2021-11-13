#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #define TERMINAL_TARGET_WINDOWS
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
    #define TERMINAL_TARGET_POSIX
#endif

#if defined(TERMINAL_TARGET_POSIX)
    #include <sys/ioctl.h>
    #include <unistd.h>

#elif defined(TERMINAL_TARGET_WINDOWS)
    #if defined(_MSC_VER)
        #if !defined(NOMINMAX)
            #define NOMINMAX
        #endif
    #endif
    #include <io.h>
    #include <windows.h>
#endif

#include <ios>
#include <iostream>
#include <type_traits>

namespace Terminal
{
#define ESCAPE_SEQUENCE "\033"
#define ANSI_SEQUENCE(NUMBER, CODE) ESCAPE_SEQUENCE "[" #NUMBER #CODE

namespace __impl
{
    static auto colorizeIndex = std::ios_base::xalloc();

    constexpr bool isColorized(std::ostream &stream) noexcept;
    constexpr bool isAtty(std::ostream &stream) noexcept;
    constexpr FILE *getStandardStream(const std::ostream &stream) noexcept;

}    // namespace __impl
inline void setupTerminal();

struct Size {
    int columns = 0;
    int lines = 0;

#if defined(TERMINAL_TARGET_WINDOWS)
    static Size get() noexcept
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;

        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        return {
            .columns = csbi.srWindow.Right - csbi.srWindow.Left + 1,
            .lines = csbi.srWindow.Bottom - csbi.srWindow.Top + 1,
        };
    }

#elif defined(TERMINAL_TARGET_POSIX)
    static Size get() noexcept
    {
        struct winsize w;

        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return {
            .columns = w.ws_col,
            .lines = w.ws_row,
        };
    }

#endif
};

enum class Style : std::uint8_t {
    Bold = 1,
    Dark = 2,
    Italic = 3,
    UnderLine = 4,
    Blink = 5,
    Reverse = 7,
    Concealed = 8,
    Crossed = 9,
};

template <Style ST>
constexpr std::ostream &style(std::ostream &stream)
{
    if (__impl::isColorized(stream)) {
        stream << ESCAPE_SEQUENCE "[" << std::to_string(static_cast<std::underlying_type_t<enum Style>>(ST)) << "m";
    }
    return stream;
}

enum class Color : std::uint8_t {
    Black = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magenta = 35,
    Cyan = 36,
    White = 37,
};

template <Color Color = Color::White>
constexpr std::ostream &color(std::ostream &stream)
{
    if (__impl::isColorized(stream)) {
        stream << ESCAPE_SEQUENCE "[" << std::to_string(static_cast<std::underlying_type_t<enum Color>>(Color)) << "m";
    }
    return stream;
}

constexpr std::ostream &reset(std::ostream &stream)
{
    if (__impl::isColorized(stream)) { stream << ESCAPE_SEQUENCE "[00m"; }
    return stream;
}

namespace __impl
{
    constexpr bool isColorized(std::ostream &stream) noexcept
    {
        return true;    // return isAtty(stream) || stream.iword(colorizeIndex);
    }

    constexpr bool isAtty(std::ostream &stream) noexcept
    {
        auto std_stream = getStandardStream(stream);

        if (!std_stream) return false;

#if defined(TERMINAL_TARGET_POSIX)
        return isatty(fileno(std_stream));
#elif define(TERMINAL_TARGET_WINDOWS)
        return ::_isatty(_fileno(std_stream));
#else
        return false;
#endif
    }

    constexpr FILE *getStandardStream(const std::ostream &stream) noexcept
    {
        if (&stream == &std::cout)
            return stdout;
        else if ((&stream == &std::cerr) || (&stream == &std::clog))
            return stderr;

        return nullptr;
    };

}    // namespace __impl

inline void setupTerminal(std::ostream &stream)
{
#if defined(TERMINAL_TARGET_WINDOWS)
    HANDLE hTerminal = INVALID_HANDLE_VALUE;
    if (&stream == &std::cout)
        hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
    else if (&stream == &std::cerr)
        hTerminal = GetStdHandle(STD_ERROR_HANDLE);
    SetConsoleMode(handle, ENABLE_VIRTUAL_TERMINAL_PROCESSING);

#elif defined(TERMINAL_TARGET_POSIX)
    // Nothing to do
#endif
    // In all case, we assume the terminal can now handele colorised input
    stream.iword(__impl::colorizeIndex) = 1L;
}

}    // namespace Terminal

#undef TERMINAL_TARGET_POSIX
#undef TERMINAL_TARGET_WINDOWS