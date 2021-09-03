#define LOGGER_EXTERN_DECLARATION
#include <Logger.hpp>
#include <iostream>

#include "logger_example.hpp"

Logger logger(std::cout);

int main(void)
{
    logger.start();

    logger.err("Test") << "this is an error message, will be printed in red";
    logger.endl();

    LOGGER_WARN << "This is a warning, that will print the current position";
    LOGGER_ENDL;    // same as logger.endl();

    logger.info() << "this is an information message";
    logger.endl();

    logger.debug("debug") << "this is a debug message";
    logger.endl();
}
