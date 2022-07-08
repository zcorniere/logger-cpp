#include <iostream>

#include "Logger.hpp"

#if defined(LOGGER_EXTERN_DECLARATION_PTR)
cpplogger::Logger *logger = nullptr;
#elif defined(LOGGER_EXTERN_DECLARATION)
cpplogger::Logger logger(std::clog);
#endif
