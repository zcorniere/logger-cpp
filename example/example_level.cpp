#include <Logger.hpp>

#include <iostream>

cpplogger::Logger logger(std::cout);

void printMessage(cpplogger::Level level)
{
    logger.setLevel(level);
    logger.debug() << "This is a debug message";
    logger.info() << "This is an information message";
    logger.warn() << "This is a warning message";
    logger.err() << "This is an error message";
    logger.msg() << "This is a message";
    logger.endl();
}

int main(void)
{
    logger.start();

    logger.msg() << "Debug | Info | Warn | Error | Message";
    printMessage(cpplogger::Level::Debug);

    logger.msg() << "Info | Warn | Error | Message";
    printMessage(cpplogger::Level::Info);

    logger.msg() << " Warn | Error | Message";
    printMessage(cpplogger::Level::Warn);

    logger.msg() << "Error | Message";
    printMessage(cpplogger::Level::Error);

    logger.msg() << "Message";
    printMessage(cpplogger::Level::Message);
}
