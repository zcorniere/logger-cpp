#pragma once

#include <string_view>

#include "cpplogger/types/ColorCodes.hpp"

namespace cpplogger
{

enum class Level {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Fatal = 5,
};

constexpr std::string_view to_string(const Level &level) noexcept
{
    switch (level) {
        case Level::Trace: return "TRACE";
        case Level::Debug: return "DEBUG";
        case Level::Info: return "INFO";
        case Level::Warn: return "WARN";
        case Level::Error: return "ERROR";
        case Level::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}

constexpr ColorPair levelColor(const Level &level)
{

    switch (level) {
        case Level::Trace: return ColorPair{.fg = Color::Green, .bg = Color::Black};
        case Level::Debug: return ColorPair{.fg = Color::Magenta, .bg = Color::Black};
        case Level::Info: return ColorPair{.fg = Color::Cyan, .bg = Color::Black};
        case Level::Warn: return ColorPair{.fg = Color::Yellow, .bg = Color::Black};
        case Level::Error: return ColorPair{.fg = Color::Red, .bg = Color::Black};
        case Level::Fatal: return ColorPair{.fg = Color::White, .bg = Color::Red};
        default: return ColorPair{.fg = Color::White, .bg = Color::Black};
    }
}

}    // namespace cpplogger
