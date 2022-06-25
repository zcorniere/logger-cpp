#define LOGGER_ACCESS .
#include <Logger.hpp>

#include <iostream>
#include <thread>

cpplogger::Logger logger(std::cout);

int main(void)
{
    const unsigned total = 5;

    logger.start();

    logger.err("Test") << "this is an error message, will be printed in red";

    auto thread1 = std::jthread([&] {
        logger.info("Thread 1") << "Started";

        auto bar = logger.newProgressBar("Thread 1 Bar", total,
                                         cpplogger::ProgressBar::Style{
                                             .bShowTime = true,
                                         });
        for (unsigned i = 0; i < total; i++) {
            ++bar;
            std::this_thread::sleep_for(std::chrono::microseconds(712345));
        }
        logger.info(bar.getMessage()) << "this is an information message";
        
        logger.deleteProgressBar(bar);
        logger.info("Thread 1") << "Ended";
    });

    auto bar2 = logger.newProgressBar("Shared Bar", total * 3);
    auto thread2 = std::jthread([&] {
        logger.info("Thread 2") << "Started";

        bool bRewind = false;
        auto bar3 = logger.newProgressBar("Thread 2 Bar", total + 3);
        while (!bar3.isComplete()) {
            ++bar2;
            ++bar3;
            if (!bRewind && bar3.getProgress() == total + 1) {
                logger.err(bar3.getMessage()) << "Something went wrong, rewinding to " << total - 1;
                bar3.setProgress(total - 1);
                bRewind = true;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(712345));
        }
        logger.deleteProgressBar(bar3);
        logger.info("Thread 2") << "Ended";
    });
    while (!bar2.isComplete()) {
        ++bar2;
        std::this_thread::sleep_for(std::chrono::microseconds(712345));
    }
    logger.deleteProgressBar(bar2);
}
