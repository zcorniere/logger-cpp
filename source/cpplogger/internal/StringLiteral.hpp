#pragma once

#include <algorithm>

namespace cpplogger::internal
{

template <std::size_t N>
class StringLiteral
{
public:
    consteval StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }

    char value[N];
};

}    // namespace cpplogger::internal
