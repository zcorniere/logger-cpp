#pragma once

#include "define.hpp"

#include <filesystem>
#include <string_view>

#if defined(LOGGER_SOURCE_LOCATION_AVAILABLE)

constexpr std::string_view
function_name(const std::source_location &location = std::source_location::current()) noexcept
{
    return location.function_name();
}

inline std::string file_position(const std::source_location &location = std::source_location::current()) noexcept
{
    return std::string() + std::filesystem::relative(location.file_name(), std::filesystem::current_path()).string() +
           ":" + std::to_string(location.line()) + ":" + std::to_string(location.column());
}

#endif

#if defined(__PRETTY_FUNCTION__)
    #define function_name() __PRETTY_FUNCTION_
#else
    #define function_name() __func__
#endif

#define STR2(x) #x
#define STR(x) STR2(x)
#define file_position() \
    std::string() + std::filesystem::relative(__FILE__, std::filesystem::current_path()).string() + ":" STR(__LINE__)
