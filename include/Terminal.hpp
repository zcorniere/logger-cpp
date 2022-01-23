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
    inline int colorizeIndex();
    inline bool isColorized(std::ostream &stream) noexcept;
    constexpr bool isAtty(std::ostream &stream) noexcept;
    constexpr FILE *getStandardStream(const std::ostream &stream) noexcept;

}    // namespace __impl

inline std::ostream &colorize(std::ostream &stream);

inline std::ostream &nocolorize(std::ostream &stream);

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
inline std::ostream &style(std::ostream &stream)
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
inline std::ostream &color(std::ostream &stream)
{
    if (__impl::isColorized(stream)) {
        stream << ESCAPE_SEQUENCE "[" << std::to_string(static_cast<std::underlying_type_t<enum Color>>(Color)) << "m";
    }
    return stream;
}

inline std::ostream &reset(std::ostream &stream)
{
    if (__impl::isColorized(stream)) { stream << ESCAPE_SEQUENCE "[00m"; }
    return stream;
}

namespace __impl
{
    inline int colorizeIndex()
    {
        static int colorize_index = std::ios_base::xalloc();
        return colorize_index;
    }

    inline bool isColorized(std::ostream &stream) noexcept
    {
        return isAtty(stream) || static_cast<bool>(stream.iword(colorizeIndex()));
    }

    constexpr bool isAtty(std::ostream &stream) noexcept
    {
        auto std_stream = getStandardStream(stream);

        if (!std_stream) return false;

#if defined(TERMINAL_TARGET_POSIX)
        return isatty(fileno(std_stream));
#elif defined(TERMINAL_TARGET_WINDOWS)
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

inline std::ostream &colorize(std::ostream &stream)
{
    stream.iword(__impl::colorizeIndex()) = 1L;
    return stream;
}

inline std::ostream &nocolorize(std::ostream &stream)
{
    stream.iword(__impl::colorizeIndex()) = 0L;
    return stream;
}

inline void setupTerminal(std::ostream &stream)
{
#if defined(TERMINAL_TARGET_WINDOWS)
#elif defined(TERMINAL_TARGET_POSIX)
#endif
    // In all case, we assume the terminal can now handle colorised input
    colorize(stream);
}

}    // namespace Terminal

#undef TERMINAL_TARGET_POSIX
#undef TERMINAL_TARGET_WINDOWS
