#include <Logger.hpp>
#include <iostream>

cpplogger::Logger logger(std::cout);

std::vector<std::string> test{"This is a test array", "Only used for testing", "aspaihd"};

struct A {
    /// This message will be prefixed by a "TRHOW" to indicated this it was emited durring stack unwinding
    ~A() { logger.err("A") << "A is destroyed"; }
};

void throw_catch_a()
try {
    A a;
    throw std::exception();
} catch (const std::exception &e) {
}

int main(void)
{
    const auto total = 100;

    logger.start();

    throw std::runtime_error("I am too tired to carry on");
    return 0;
}
