#pragma once

#include "utils/sequences.hpp"

namespace cpplogger
{

enum class Level {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Message,
};

std::string to_string(const Level &level) noexcept;

Color levelColor(const Level &level) noexcept;

}    // namespace cpplogger