#pragma once

#if __has_include(<cxxabi.h>)
    #define LOGGER_CXXABI_AVAILABLE
#endif

#if __has_include(<source_location>)
    #include <source_location>

    #if defined(__cpp_lib_source_location)
        #define LOGGER_SOURCE_LOCATION_AVAILABLE
    #endif

#endif

#if defined(_WIN32) || defined(_WIN64)
    #define TERMINAL_TARGET_WINDOWS
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
    #define TERMINAL_TARGET_POSIX
#endif

#if defined(TERMINAL_TARGET_POSIX)
    #include <sys/ioctl.h>
    #include <unistd.h>

#elif defined(TERMINAL_TARGET_WINDOWS)
    #define NOMINMAX

    #include <io.h>
    #include <windows.h>
#endif
