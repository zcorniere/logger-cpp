#pragma once

namespace cpplogger
{

// add +30 to have the ANSI color code
// order is important !
enum class Color : std::uint8_t {
    Black = 0,
    Red = 1,
    Green = 2,
    Yellow = 3,
    Blue = 4,
    Magenta = 5,
    Cyan = 6,
    White = 7,
};

struct ColorPair {
    Color fg : 4;
    Color bg : 4;
};
static_assert(sizeof(ColorPair) == 1);

}    // namespace cpplogger