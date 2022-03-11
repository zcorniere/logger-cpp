#include "Terminal.hpp"

namespace Terminal
{

namespace __impl
{
    int colorizeIndex()
    {
        static int colorize_index = std::ios_base::xalloc();
        return colorize_index;
    }

    bool isColorized(std::ostream &stream) noexcept
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

std::ostream &reset(std::ostream &stream)
{
    if (__impl::isColorized(stream)) { stream << LOGGER_ESCAPE_SEQUENCE "[00m"; }
    return stream;
}

std::ostream &colorize(std::ostream &stream)
{
    stream.iword(__impl::colorizeIndex()) = 1L;
    return stream;
}

std::ostream &nocolorize(std::ostream &stream)
{
    stream.iword(__impl::colorizeIndex()) = 0L;
    return stream;
}

void setupTerminal(std::ostream &stream)
{
#if defined(TERMINAL_TARGET_WINDOWS)
#elif defined(TERMINAL_TARGET_POSIX)
#endif
    // In all case, we assume the terminal can now handle colorised input
    colorize(stream);
}

}    // namespace Terminal