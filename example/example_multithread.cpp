#define LOGGER_EXTERN_DECLARATION
#include <Logger.hpp>
#include <iostream>
#include <thread>

Logger logger(std::cout);

int main(void)
{
    const unsigned total = 5;

    logger.start();

    logger.err("Test") << "this is an error message, will be printed in red";
    logger.endl();

    LOGGER_WARN << "This is a warning, that will print the current position";
    LOGGER_ENDL;    // same as logger.endl();

    auto thread1 = std::jthread([&] {
        logger.info("Thread 1") << "Started";
        LOGGER_ENDL;

        auto bar = logger.newProgressBar("Thread 1 Bar", total, true);
        for (unsigned i = 0; i < total; i++) {
            ++bar;
            std::this_thread::sleep_for(std::chrono::microseconds(712345));
        }
        logger.info(bar.getMessage()) << "this is an information message";
        logger.endl();
        logger.deleteProgressBar(bar);
        logger.info("Thread 1") << "Ended";
        LOGGER_ENDL;
    });

    auto bar2 = logger.newProgressBar("Shared Bar", total * 3);
    auto thread2 = std::jthread([&] {
        logger.info("Thread 2") << "Started";
        LOGGER_ENDL;

        bool bRewind = false;
        auto bar3 = logger.newProgressBar("Thread 2 Bar", total + 3);
        while (!bar3.isComplete()) {
            ++bar2;
            ++bar3;
            if (!bRewind && bar3.getProgress() == total + 1) {
                logger.err(bar3.getMessage()) << "Something went wrong, rewinding to " << total - 1;
                LOGGER_ENDL;
                bar3.setProgress(total - 1);
                bRewind = true;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(712345));
        }
        logger.deleteProgressBar(bar3);
        logger.info("Thread 2") << "Ended";
        LOGGER_ENDL;
    });
    while (!bar2.isComplete()) {
        ++bar2;
        std::this_thread::sleep_for(std::chrono::microseconds(712345));
    }
    logger.deleteProgressBar(bar2);
}
