#include <cpplogger/Logger.hpp>
#include <cpplogger/formatters/DefaultFormatter.hpp>
#include <cpplogger/sinks/StdoutSink.hpp>

#include <cstdio>

DELARE_LOGGER_CATEGORY(Example, TestCategory, Level::Error)

int main(int ac, char **av)
{
    cpplogger::Logger logger("Example");

    logger.addSink(new cpplogger::StdoutSink(stdout), new cpplogger::DefaultFormatter);

    TestCategory::log(cpplogger::Level::Info, "number of args {}", ac);
}
