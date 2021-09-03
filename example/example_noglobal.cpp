#include <Logger.hpp>
#include <iostream>

int main(void)
{
    const auto total = 5;

    Logger logger(std::cout);
    logger.start();

    logger.err("Test") << "this is an error message, will be printed in red";
    logger.endl();

    {
        auto &bar = logger.newProgressBar("Bar", total, true);
        for (unsigned i = 0; i < total; i++) {
            ++bar;
            usleep(712345);
        }
        logger.info(bar.getMessage()) << "this is an information message";
        logger.endl();
        logger.deleteProgressBar(bar);
    }

    {
        bool bRewind = false;
        auto &bar2 = logger.newProgressBar("Bar2", total);
        auto &bar3 = logger.newProgressBar("Bar3", total + 3);
        while (!bar2.isComplete() || !bar3.isComplete()) {
            ++bar2;
            ++bar3;
            if (bar2.isComplete()) {
                logger.debug("debug") << "this is a debug message";
                logger.endl();
            }
            if (!bRewind && bar3.getProgress() == total + 1) {
                logger.err(bar3.getMessage()) << "Something went wrong, rewinding to " << total - 1;
                logger.endl();
                bar3.setProgress(total - 1);
                bRewind = true;
            }
            usleep(712345);
        }
        logger.deleteProgressBar(bar2, bar3);
    }
}
