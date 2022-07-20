#include <cpplogger/Logger.hpp>
#include <cpplogger/utils/mutex.hpp>
#include <cpplogger/utils/source_location.hpp>

#include <barrier>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <vector>

constexpr auto file_path("file.txt");
constexpr auto nbOfWorker = 8;
constexpr auto nbOfLinePrinted = 100000;

cpplogger::Logger logger(std::clog);
cpplogger::mutex<std::ofstream> file(file_path);

class ScopedTime
{
public:
    ScopedTime(const std::string message) noexcept: message(std::move(message)) { startTimer(); }
    ~ScopedTime() { endTimer(); }

    void startTimer() noexcept { start = std::chrono::high_resolution_clock::now(); }
    void endTimer() const
    {
        auto end = std::chrono::high_resolution_clock::now();
        file.lock([this, &end](auto &i) {
            std::chrono::duration<double> elapsedTime(end - start);
            i << message << " => " << elapsedTime.count() << "s" << std::endl;
        });
    }

private:
    const std::string message;
    std::chrono::high_resolution_clock::time_point start;
};

void print_lines(const std::string &msg = {}) noexcept
{
    ScopedTime time("Printing " + std::to_string(nbOfLinePrinted) + " lines {" + msg + "}");
    for (int i = 0; i <= nbOfLinePrinted; i++) { logger.debug(msg) << i; }
}

int main(void)
{
    ScopedTime globalTime(function_name());
    logger.start();

    print_lines();

    {
        {
            ScopedTime time("Multithread");
            std::barrier sync_point(nbOfWorker, [&time]() noexcept { time.startTimer(); });

            auto work = [&](int id) {
                const auto msg = "Multithread/" + std::to_string(id);
                sync_point.arrive_and_wait();
                print_lines(msg);
            };
            std::vector<std::jthread> workers(nbOfWorker);
            for (unsigned i = 0; i < nbOfWorker; i++) workers.emplace_back(work, i);
        }
    }
}
