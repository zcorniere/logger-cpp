#include <Logger.hpp>
#include <iostream>

int main(void)
{
    const auto total = 5;

    Logger logger(std::cout);
    logger.start();

    logger.err("Test") << "this is an error message, will be printed in red";

    LOGGER_WARN(logger) << "This is a warning, that will print the current position";
    {
        bool bRewind = false;
        auto bar2 = logger.newProgressBar("Bar2", total);
        auto bar3 = logger.newProgressBar("Bar3", total + 3);
        while (!bar2.isComplete() || !bar3.isComplete()) {
            ++bar2;
            ++bar3;
            if (bar2.isComplete()) {
                logger.debug("debug") << "this is a debug message";
                
            }
            if (!bRewind && bar3.getProgress() == total + 1) {
                logger.err(bar3.getMessage()) << "Something went wrong, rewinding to " << total - 1;

                bar3.setProgress(total - 1);
                bRewind = true;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(712345));
        }
        logger.deleteProgressBar(bar2, bar3);
    }
}
