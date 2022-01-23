#define LOGGER_EXTERN_DECLARATION
#include <Logger.hpp>
#include <iostream>

Logger logger(std::cout);

int main(void)
{
    const auto total = 100;

    logger.start();

    logger.err("Test") << "this is an error message, will be printed in red";

    LOGGER_WARN << "This is a warning, that will print the current position";

    {
        auto bar = logger.newProgressBar("Bar with a really long title", total,
                                         ProgressBar::Style{
                                             .bShowTime = true,
                                         });
        for (unsigned i = 0; i < total; i++) {
            ++bar;
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        logger.info(bar.getMessage()) << "this is an information message";

        logger.deleteProgressBar(bar);
    }

    {
        bool bRewind = false;
        auto bar2 = logger.newProgressBar("Bar2", total,
                                          ProgressBar::Style{
                                              .cFill = '#',
                                              .cEqual = '#',
                                              .cEmpty = '-',
                                          });
        auto bar3 = logger.newProgressBar("Bar3", total);
        while (!bar2.isComplete() || !bar3.isComplete()) {
            ++bar2;
            ++bar3;
            if (bar2.isComplete() && bar3.getProgress() > 80) { logger.debug("debug") << "this is a debug message"; }
            if (!bRewind && bar3.getProgress() == total - 10) {
                logger.err(bar3.getMessage()) << "Something went wrong, rewinding to " << total / 2;
                bar3.setProgress(total / 2);
                bRewind = true;
            }
            if (!bar2.isComplete()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(120));
            }
        }
        logger.deleteProgressBar(bar2, bar3);
    }
}
