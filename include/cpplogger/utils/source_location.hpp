#pragma once

#include "cpplogger/define.hpp"

#include <filesystem>
#include <string>
#include <string_view>

namespace
{

inline std::string __function_name(const char *const function) { return std::string(function); }

inline std::string __file_position(const char *const file, int line, int col = -1)
{
    return std::string() + ::std::filesystem::relative(file, ::std::filesystem::current_path()).string() + ":" +
           std::to_string(line) + ((col == -1) ? ("") : (":" + std::to_string(col)));
}

}    // namespace

#if defined(LOGGER_SOURCE_LOCATION_AVAILABLE)

inline std::string function_name(const std::source_location &location = std::source_location::current())
{
    return __function_name(location.function_name());
}

inline std::string file_position(const std::source_location &location = std::source_location::current())
{
    return __file_position(location.file_name(), location.line(), location.column());
}

#else

    #if defined(__GNUC__) || defined(__clang__)
        #define function_name() __function_name(__PRETTY_FUNCTION__)
    #elif defined(__FUNCSIG__)
        #define function_name() __function_name(__FUNCSIG__)
    #else
        #define function_name() __function_name(__FUNCTION__)
    #endif

    #define file_position() __file_position(__FILE__, __LINE__)

#endif
