#include <cpplogger/Logger.hpp>
#include <cpplogger/formatters/ColorFormatter.hpp>
#include <cpplogger/sinks/StdoutSink.hpp>

#include <cstdio>
#include <thread>

DECLARE_LOGGER_CATEGORY(Example, TestCategory, Info)

int main(int ac, char **av)
{
    cpplogger::Logger logger("Example");

    logger.addSink(std::make_unique<cpplogger::StdoutSink>(stdout), std::make_unique<cpplogger::ColorFormatter>());

    std::vector<std::jthread> threads;

    for (unsigned j = 0; j < 5; j++) {

        threads.push_back(std::jthread([ac]() {
            for (unsigned i = 0; i < 1000; i++) {
                LOG(TestCategory, Info, "Info text {}", ac);
                LOG(TestCategory, Error, "Error text");
                LOG(TestCategory, Fatal, "LOL");
            }
        }));
    }
}
