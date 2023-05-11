#pragma once

#include <string_view>

#include "cpplogger/types/ColorCodes.hpp"

namespace cpplogger
{

enum class Level {
    Nothing = 0,
    Fatal,
    Error,
    Warning,
    Info,
    Trace,
    NumVerbority,
};

constexpr std::string_view to_string(const Level &level) noexcept
{
    switch (level) {
        case Level::Nothing: return "Nothing";
        case Level::Fatal: return "Fatal";
        case Level::Error: return "Trace";
        case Level::Warning: return "Warning";
        case Level::Info: return "Info";
        case Level::Trace: return "Trace";
        default: return "Unkown";
    }
}

constexpr ColorPair levelColor(const Level &level)
{
    switch (level) {
        case Level::Nothing: return ColorPair{.fg = Color::Red, .bg = Color::White};
        case Level::Fatal: return ColorPair{.fg = Color::White, .bg = Color::Red};
        case Level::Error: return ColorPair{.fg = Color::Red, .bg = Color::Black};
        case Level::Warning: return ColorPair{.fg = Color::Yellow, .bg = Color::Black};
        case Level::Info: return ColorPair{.fg = Color::Cyan, .bg = Color::Black};
        case Level::Trace: return ColorPair{.fg = Color::Green, .bg = Color::Black};
        default: return ColorPair{.fg = Color::White, .bg = Color::Black};
    }
}

}    // namespace cpplogger
