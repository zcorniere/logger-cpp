#define LOGGER_EXTERN_DECLARATION
#include <Logger.hpp>
#include <iostream>

Logger logger(std::cout);

void printMessage(Logger::Level level)
{
    logger.setLevel(level);
    logger.debug() << "This is a debug message";

    logger.info() << "This is an information message";

    logger.warn() << "This is a warning message";

    logger.err() << "This is an error message";

    logger.msg() << "This is a message";
}

int main(void)
{
    logger.start();

    logger.msg() << "Debug | Info | Warn | Error | Message";
    printMessage(Logger::Level::Debug);

    logger.msg() << "Info | Warn | Error | Message";
    printMessage(Logger::Level::Info);

    logger.msg() << " Warn | Error | Message";
    printMessage(Logger::Level::Warn);

    logger.msg() << "Error | Message";
    printMessage(Logger::Level::Error);

    logger.msg() << "Message";
    printMessage(Logger::Level::Message);
}
