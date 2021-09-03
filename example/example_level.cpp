#define LOGGER_EXTERN_DECLARATION
#include <Logger.hpp>
#include <iostream>

#include "logger_example.hpp"

Logger logger(std::cout);

void printMessage(Logger::Level level)
{
    logger.setLevel(level);
    logger.debug() << "This is a debug message";
    LOGGER_ENDL;

    logger.info() << "This is an information message";
    LOGGER_ENDL;

    logger.warn() << "This is a warning message";
    LOGGER_ENDL;

    logger.err() << "This is an error message";
    LOGGER_ENDL;

    logger.msg() << "This is a message";
    LOGGER_ENDL;

    LOGGER_ENDL;
}

int main(void)
{
    logger.start();

    logger.msg() << "Debug | Info | Warn | Error | Message";
    LOGGER_ENDL;
    printMessage(Logger::Level::Debug);

    logger.msg() << "Info | Warn | Error | Message";
    LOGGER_ENDL;
    printMessage(Logger::Level::Info);

    logger.msg() << " Warn | Error | Message";
    LOGGER_ENDL;
    printMessage(Logger::Level::Warn);

    logger.msg() << "Error | Message";
    LOGGER_ENDL;
    printMessage(Logger::Level::Error);

    logger.msg() << "Message";
    LOGGER_ENDL;
    printMessage(Logger::Level::Message);
}
