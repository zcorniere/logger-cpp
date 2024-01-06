#pragma once

#include <cstdint>
#include <string>

namespace cpplogger
{

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
    Color fg : 4 = Color::White;
    Color bg : 4 = Color::Black;
};
static_assert(sizeof(ColorPair) == 1);

namespace internal
{

    constexpr static auto resetSequence = "\033[0m";

    constexpr std::string color(const ColorPair &color)
    {
        const std::string BaseCode("\033[30m\033[40m");

        std::string code(BaseCode);
        code[3] = char(static_cast<std::uint8_t>(color.fg) + 48);
        code[8] = char(static_cast<std::uint8_t>(color.bg) + 48);
        return code;
    }

}    // namespace internal

}    // namespace cpplogger
