#pragma once

#include "cpplogger/define.hpp"

#if defined(LOGGER_CXXABI_AVAILABLE)
    #include <cxxabi.h>
#endif

#include <optional>
#include <stdexcept>
#include <string>

namespace cpplogger
{

inline std::string demangle(char const *name);
inline std::optional<std::string> demangle_exception_ptr(const std::exception_ptr ep);

#if defined(LOGGER_CXXABI_AVAILABLE)
inline std::string demangle(char const *name)
{

    int status = 0;
    std::size_t size = 0;
    auto *p = abi::__cxa_demangle(name, NULL, &size, &status);
    if (status != 0) throw std::runtime_error("Demangleling operation failed for name \"" + std::string(name) + "\"");

    std::string ret(p, size);
    std::free(p);
    return ret;
}

inline std::optional<std::string> demangle_exception_ptr(const std::exception_ptr ep)
{
    if (!ep) return std::nullopt;
    auto const etype = abi::__cxa_current_exception_type()->name();
    return demangle(etype);
}

#else
inline std::string demangle(char const *name) { return std::string(name); }
inline std::optional<std::string> demangle_exception_ptr(const std::exception_ptr) { return "std::exception"; }
#endif

}    // namespace cpplogger

#define DEMANGLE_THIS ::cpplogger::demangle(typeid(*this).name())
