#pragma once

#include <chrono>
#include <source_location>
#include <string>
#include <string_view>

#include "cpplogger/types/Level.hpp"

namespace cpplogger
{

struct Message {
    const Level LogLevel = Level::Trace;
    const std::string_view CategoryName;
    const std::string Message;
    const std::source_location LogLocation = std::source_location::current();
    const std::chrono::system_clock::time_point LogTime = std::chrono::system_clock::now();
};

}    // namespace cpplogger