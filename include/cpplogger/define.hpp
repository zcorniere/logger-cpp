#pragma once

#if __has_include(<cxxabi.h>)
    #define LOGGER_CXXABI_AVAILABLE
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

#if (defined(__clang__) || defined(__GNUC__))
    #define LOGGER_FORCEINLINE inline __attribute__((always_inline)) /* Force code to be inline */
    #define LOGGER_FORCENOINLINE __attribute__((noinline))           /* Force code to NOT be inline */
#elif defined(_MSC_VER)
    #define LOGGER_FORCEINLINE inline __forceinline /* Force code to be inline */
    #define LOGGER_FORCENOINLINE __declspec(noinline)
#else
    #define LOGGER_FORCEINLINE inline
    #define LOGGER_FORCENOINLINE
#endif