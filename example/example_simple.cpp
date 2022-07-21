#include <cpplogger/Logger.hpp>
#include <cpplogger/utils/source_location.hpp>

#include <iostream>

cpplogger::Logger logger(std::cout);

int main(void)
{
    logger.start();

    logger.err("Test") << "this is an error message, will be printed in red";

    logger.warn(file_position()) << "This is a warning, that will print the current position";

    logger.info() << "this is an information message";

    logger.debug(function_name()) << "this is a debug message";
}
