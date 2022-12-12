#pragma once

#include "cpplogger/define.hpp"

#include <string>
#include <string_view>
#include <vector>

#define LOGGER_ESCAPE_SEQUENCE "\033"

namespace cpplogger
{

const std::vector<const std::string_view> FormattingCodes{
    "\033[m",     // reset
    "\033[1m",    // bold
    "\033[2m",    // dark
    "\033[4m",    // underline
    "\033[5m",    // blink
    "\033[7m",    // reverse
    "\033[8m",    // concealed
    "\033[K",     // clear_line
};

enum Style {
    Reset = 0,
    Bold = 1,
    Dark = 2,
    UnderLine = 3,
    Blink = 4,
    Reverse = 5,
    Concealed = 6,
    Crossed = 7,
};

const std::vector<const std::string_view> ColorCodes{
    "\033[30m",    // Black
    "\033[31m",    // Red
    "\033[32m",    // Green
    "\033[33m",    // Yellow
    "\033[34m",    // Blue
    "\033[35m",    // Magenta
    "\033[36m",    // Cyan
    "\033[37m",    // White
};

enum Color {
    Black = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magenta = 35,
    Cyan = 36,
    White = 37,
};

const std::vector<const std::string_view> OnColorCodes{
    "\033[40m",    // OnBlack
    "\033[41m",    // OnRed
    "\033[42m",    // OnGreen
    "\033[43m",    // OnYellow
    "\033[44m",    // OnBlue
    "\033[45m",    // OnMagenta
    "\033[46m",    // OnCyan
    "\033[47m",    // OnWhite
};

enum Color {
    OnBlack = 40,
    OnRed = 41,
    OnGreen = 42,
    OnYellow = 43,
    OnBlue = 44,
    OnMagenta = 45,
    OnCyan = 46,
    OnWhite = 47,
};

struct ColorPair {
    Color foreground;
    Color Backgroud;
};

}    // namespace cpplogger