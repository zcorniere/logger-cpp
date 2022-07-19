#define LOGGER_EXTERN_DECLARATION
#include <Logger.hpp>
#include <types/ProgressBar.hpp>
#include <types/Spinner.hpp>

#include <iostream>

using namespace cpplogger;
using namespace std::literals;

Logger logger(std::clog);

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

    logger.start();

    throw_catch_a();

    logger.err("Test") << "this is an error message, will be printed in red";

    logger.warn() << "This is a warning, that will print the current position";

    logger.debug("Test vector") << test;
    {
        const auto total = 3000;
        auto bar = logger.add<cpplogger::ProgressBar>("Bar with a really long title", total,
                                                      ProgressBar::Style{
                                                          .bShowTime = true,
                                                      });
        for (unsigned i = 0; !bar->isComplete(); i++) {
            bar->addProgress(i);
            std::this_thread::sleep_for(30ms);
        }
        logger.info(bar->getMessage()) << "this is an information message";
        logger.remove(bar);
    }

    {
        const auto total = 100;
        auto spinner = logger.add<cpplogger::Spinner>("Spin to win, my boy. You shall zoom pass them.");
        auto bar2 = logger.add<cpplogger::ProgressBar>("Bar2", total * 3,
                                                       ProgressBar::Style{
                                                           .cFill = '#',
                                                           .cEqual = '#',
                                                           .cEmpty = '-',
                                                       });
        auto bar3 = logger.add<cpplogger::ProgressBar>("Bar3", total);
        while (!bar2->isComplete()) {
            bar2->addProgress(3);
            bar3->addProgress(1);
            if (bar3->getProgress() == total - 10) {
                logger.err(bar3->getMessage()) << "Something went wrong, rewinding to " << total / 2;
                bar3->setProgress(total / 2);
            }
            std::this_thread::sleep_for(60ms);
        }
        spinner->setComplete();

        while (!bar3->isComplete()) {
            bar3->addProgress(1);
            if (bar3->getProgress() > 80) { logger.debug() << "this is a debug message"; }
            std::this_thread::sleep_for(30ms);
        }
        logger.remove(bar2, bar3, spinner);
    }
}