#include <Logger.hpp>

#include <barrier>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <vector>

constexpr auto file_path("file.txt");

cpplogger::Logger logger(std::clog);

#define START_POINT(msg)                                    \
    auto start = std::chrono::high_resolution_clock::now(); \
    file << "end " msg " : " << start.time_since_epoch().count() << "s" << std::endl;

#define END_POINT(msg)                                                                \
    auto end = std::chrono::high_resolution_clock::now();                             \
    file << "start " msg " : " << end.time_since_epoch().count() << "s" << std::endl; \
    std::chrono::duration<double> elapsedTime(end - start);                           \
    file << msg " took : " << elapsedTime.count() << "s" << std::endl;

#define TEST_SCOPE(msg)   \
    {                     \
        START_POINT(msg); \
        print_bs(msg);    \
        END_POINT(msg);   \
    }

void print_bs(const std::string &msg) noexcept
{
    for (int i = 0; i < 1000000; i++) { logger.debug(msg) << i; }
}

int main(void)
{
    std::ofstream file((std::filesystem::path(file_path)));
    logger.start();

    TEST_SCOPE("1000000")

    {
        std::barrier sync_point(8, []() noexcept { print_bs("Multithread 1000000"); });

        auto work = [&sync_point]() { sync_point.arrive_and_wait(); };

        START_POINT("Multithread 1000000");

        std::vector<std::jthread> workers;
        for (unsigned i = 0; i < 8; i++) { workers.emplace_back(work); }

        END_POINT("Multithread 1000000");
    }
}
