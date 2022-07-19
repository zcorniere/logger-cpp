#include <cpplogger/Logger.hpp>

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
    file << "start " << msg << " : " << start.time_since_epoch().count() << "s" << std::endl;

#define END_POINT(msg)                                                                    \
    auto end = std::chrono::high_resolution_clock::now();                                 \
    file << "end " << msg << " : " << end.time_since_epoch().count() << "s" << std::endl; \
    std::chrono::duration<double> elapsedTime(end - start);                               \
    file << msg << " took : " << elapsedTime.count() << "s" << std::endl;

#define TEST_SCOPE(msg)   \
    {                     \
        START_POINT(msg); \
        print_bs(msg);    \
        END_POINT(msg);   \
    }

const auto nbOfWorker = 8;
const auto nbOfLinePrinted = 100000;

void print_bs(const std::string &msg) noexcept
{
    for (int i = 0; i <= nbOfLinePrinted; i++) { logger.debug(msg) << i; }
}

int main(void)
{
    std::ofstream file((std::filesystem::path(file_path)));
    logger.start();

    TEST_SCOPE(std::to_string(nbOfLinePrinted))
    file << std::endl << std::endl;
    {
        START_POINT("Multithread " + std::to_string(nbOfLinePrinted));
        std::barrier sync_point(nbOfWorker, [&]() noexcept { start = std::chrono::high_resolution_clock::now(); });

        auto work = [&](int id) {
            const auto msg = "Multithread/" + std::to_string(id) + " " + std::to_string(nbOfLinePrinted);
            sync_point.arrive_and_wait();
            print_bs(msg);
            END_POINT(msg);
        };

        std::vector<std::jthread> workers;
        for (unsigned i = 0; i < nbOfWorker; i++) { workers.emplace_back(work, i); }

        END_POINT("Multithread " + std::to_string(nbOfLinePrinted));
    }
}
