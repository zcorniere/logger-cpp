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

    throw_catch_a();

    logger.err("Test") << "this is an error message, will be printed in red";

    logger.warn() << "This is a warning, that will print the current position";

    logger.debug("Test vector") << test;
    {
        auto bar = logger.newProgressBar("Bar with a really long title", total,
                                         cpplogger::ProgressBar::Style{
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
                                          cpplogger::ProgressBar::Style{
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
