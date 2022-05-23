#pragma once

#include "define.hpp"

#include <string>
#include <string_view>

namespace cpplogger
{

enum class Style {
    Bold = 1,
    Dark = 2,
    Italic = 3,
    UnderLine = 4,
    Blink = 5,
    Reverse = 7,
    Concealed = 8,
    Crossed = 9,
};

enum class Color {
    Black = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magenta = 35,
    Cyan = 36,
    White = 37,
};

inline std::string style(const Style &sty)
{
    return LOGGER_ESCAPE_SEQUENCE "[" + std::to_string(static_cast<std::underlying_type_t<enum Style>>(sty)) + "m";
}

inline std::string color(const Color &color)
{
    return LOGGER_ESCAPE_SEQUENCE "[" + std::to_string(static_cast<std::underlying_type_t<enum Color>>(color)) + "m";
}

inline std::string repeat(unsigned n, const std::string &s)
{
    std::string result;
    result.reserve(n * s.size());
    for (unsigned i = 0; i < n; ++i) { result += s; }
    return result;
}

inline std::string reset() { return LOGGER_ESCAPE_SEQUENCE "[0m"; }

inline std::string clearBeforeCursor() { return LOGGER_ESCAPE_SEQUENCE "[0K"; }

inline std::string clearAfterCursor() { return LOGGER_ESCAPE_SEQUENCE "[1K"; }

inline std::string clearLine() { return LOGGER_ESCAPE_SEQUENCE "[2K\r"; }
inline std::string moveUp(unsigned n = 1) { return LOGGER_ESCAPE_SEQUENCE "[" + std::to_string(n) + "A\r"; }

inline std::string clearLines(unsigned n = 1)
{
    return LOGGER_ESCAPE_SEQUENCE "[0m" + clearBeforeCursor() +
           ((n) ? repeat(n, clearLine() + moveUp()) : std::string(""));
}

}    // namespace cpplogger