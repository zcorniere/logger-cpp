#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
    #define PLATFORM_UNIX
#endif

#if defined(PLATFORM_UNIX)
    #include <sys/ioctl.h>
    #include <unistd.h>

#elif defined(PLATFORM_WINDOWS)
    #define NOMINMAX

    #include <io.h>
    #include <windows.h>
#endif
