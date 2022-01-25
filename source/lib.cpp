#include <iostream>

#include "Logger.hpp"

#if defined(LOGGER_EXTERN_DECLARATION_PTR)
Logger *logger = nullptr;
#elif defined(LOGGER_EXTERN_DECLARATION)
Logger logger(std::cout);
#endif
