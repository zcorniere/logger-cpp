#pragma once

#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
#include <Windows.h>
inline void usleep(uint32_t s) { Sleep(s); }
#else
#include <unistd.h>
#endif