#include "types/Level.hpp"

namespace cpplogger
{

std::string to_string(const Level &level) noexcept
{
    switch (level) {
        case Level::Trace: return "TRACE";
        case Level::Debug: return "DEBUG";
        case Level::Info: return "INFO";
        case Level::Warn: return "WARN";
        case Level::Error: return "ERROR";
        case Level::Message: return "MESSAGE";
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
        case Level::Message: return Color::White;
        default: return Color::White;
    }
}

}    // namespace cpplogger