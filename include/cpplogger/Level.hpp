#pragma once

#include <string>

#include "cpplogger/utils/sequences.hpp"

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

Color levelColor(const Level &level) noexcept
{
    switch (level) {
        case Level::Trace: return Color::Green;
        case Level::Debug: return Color::Magenta;
        case Level::Info: return Color::Cyan;
        case Level::Warn: return Color::Yellow;
        case Level::Error: return Color::Red;
        case Level::Fatal: return Color::Red;
        default: return Color::White;
    }
}

}    // namespace cpplogger