#pragma once

#include <format>
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
        case Level::Error: return "Error";
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

template <>
struct std::formatter<cpplogger::Level, char> {
    bool isColored = false;

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext &ctx)
    {
        typename ParseContext::iterator pos = ctx.begin();
        while (pos != ctx.end() && *pos != '}') {
            if (*pos == '#') isColored = true;
            ++pos;
        }
        return pos;
    }

    template <class FmtContext>
    FmtContext::iterator format(cpplogger::Level Level, FmtContext &ctx) const
    {
        using namespace cpplogger;

        if (isColored) {
            format_to(ctx.out(), "{0:s}{1:s}{2:s}", internal::color(levelColor(Level)), to_string(Level),
                      internal::resetSequence);
        } else {
            format_to(ctx.out(), "{0:s}", to_string(Level));
        }
        return ctx.out();
    }
};
