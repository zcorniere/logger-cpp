#pragma once

#include "define.hpp"

#if defined(LOGGER_CXXABI_AVAILABLE)
    #include <cxxabi.h>
#endif

#include <stdexcept>
#include <string>

namespace cpplogger
{

inline std::string demangle(char const *name)
{
#if defined(LOGGER_CXXABI_AVAILABLE)

    int status = 0;
    std::size_t size = 0;
    auto *p = abi::__cxa_demangle(name, NULL, &size, &status);
    if (status != 0) throw std::runtime_error("Demangleling operation failed for name \"" + std::string(name) + "\"");

    std::string ret(p, size);
    std::free(p);
    return ret;
#else
    return std::string(name);
#endif
}

}    // namespace cpplogger

#define DEMANGLE_THIS ::cpplogger::demangle(typeid(*this).name())