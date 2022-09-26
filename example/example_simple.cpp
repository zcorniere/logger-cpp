#include <cpplogger/Logger.hpp>

#include <iostream>

cpplogger::Logger logger(std::cout);

int main(void)
{
    logger.err("Test") << "this is an error message, will be printed in red";

    logger.warn() << "This is a warning, that will print the current position";

    logger.info() << "this is an information message";

    logger.debug() << "this is a debug message";

    logger.print();
}
