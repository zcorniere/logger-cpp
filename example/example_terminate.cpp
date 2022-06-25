#include <Logger.hpp>
#include <iostream>

cpplogger::Logger logger(std::cout);

int main(void)
{
    const auto total = 100;

    logger.start();

    throw std::runtime_error("I am too tired to carry on");
    return 0;
}
