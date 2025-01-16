#include <cpplogger/Logger.hpp>
#include <cpplogger/formatters/ColorFormatter.hpp>
#include <cpplogger/formatters/DefaultFormatter.hpp>
#include <cpplogger/sinks/FileSink.hpp>
#include <cpplogger/sinks/StdoutSink.hpp>

#include <cstdio>
#include <thread>

DECLARE_LOGGER_CATEGORY(Example, TestCategory, Info)

DECLARE_LOGGER_CATEGORY(Example, TestMuteCateogry, Nothing);

int main(int ac, char **av)
{
    cpplogger::Logger logger("Example");
    logger.addSink<cpplogger::StdoutSink, cpplogger::ColorFormatter>(stdout);
    logger.addSink<cpplogger::FileSink, cpplogger::ColorFormatter>("TestFile_Color.txt", false);
    logger.addSink<cpplogger::FileSink, cpplogger::DefaultFormatter>("TestFile_NoColor.txt", false);

    std::vector<std::jthread> threads;
    for (unsigned j = 0; j < 20; j++) {

        threads.push_back(std::jthread([j]() {
            for (unsigned i = 0; i < 2000; i++) {
                LOG(TestMuteCateogry, Info, "Info text {}", i);
                LOG(TestMuteCateogry, Error, "Error text");
                LOG(TestMuteCateogry, Fatal, "LOL");

                LOG(TestCategory, Info, "Info text {} {}", j, i);
                LOG(TestCategory, Error, "Error text");
                LOG(TestCategory, Fatal, "LOL");
            }
        }));
    }
}
