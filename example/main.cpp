#include <Logger.hpp>

Logger *logger = nullptr;

int main(void)
{
    const auto total = 8;

    logger = new Logger(std::cout);
    logger->start();

    logger->err("Test") << "this is an error message, will be printed in red";
    logger->endl();

    LOGGER_WARN << "This is a warning, that will print the current position";
    LOGGER_ENDL;    // same as logger->endl();

    {
        auto bar = logger->newProgressBar("Bar", total);
        for (unsigned i = 0; i < total; i++) {
            ++(*bar);
            usleep(712345);
        }
        logger->deleteProgressBar(bar);
    }
    logger->debug("debug") << "this is an debug message";
    logger->endl();

    {
        auto bar2 = logger->newProgressBar("Bar2", total);
        auto bar3 = logger->newProgressBar("Bar3", total + 3);
        while (!bar2->isComplete() || !bar3->isComplete()) {
            ++(*bar2);
            ++(*bar3);
            usleep(712345);
        }
        logger->deleteProgressBar(bar2);
        logger->deleteProgressBar(bar3);
    }

    delete logger;
}
