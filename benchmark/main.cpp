#include <Logger.hpp>

#include <barrier>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <vector>

constexpr auto file_path("file.txt");

cpplogger::Logger logger(std::cout);

#define START_POINT(msg)                                    \
    auto start = std::chrono::high_resolution_clock::now(); \
    file << msg " : " << start.time_since_epoch().count() << "s" << std::endl;

#define END_POINT(msg)                                                       \
    auto end = std::chrono::high_resolution_clock::now();                    \
    file << msg " : " << end.time_since_epoch().count() << "s" << std::endl; \
    std::chrono::duration<double> elapsedTime(end - start);                  \
    file << msg " took : " << elapsedTime.count() << "s" << std::endl;

void print_bs() noexcept
{
    for (int i = 0; i < 1000000; i++) { logger.debug() << i; }
}

int main(void)
{
    std::ofstream file((std::filesystem::path(file_path)));
    logger.start();

    {
        START_POINT("start 1000000");
        print_bs();
        END_POINT("end 1000000");
    }

    {
        std::barrier sync_point(8, print_bs);

        auto work = [&sync_point]() { sync_point.arrive_and_wait(); };

        START_POINT("Multithread 1000000");

        std::vector<std::thread> workers;
        for (unsigned i = 0; i < 8; i++) { workers.emplace_back(work); }
        for (auto &worker: workers) { worker.join(); }

        END_POINT("Multithread 1000000");
    }
}