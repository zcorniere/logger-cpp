#pragma once

#include <atomic>
#include <chrono>
#include <ostream>
#include <stdint.h>
#include <string>

class ProgressBar
{
public:
    ProgressBar(std::string _message = "", uint64_t max = 100, bool show_time_ = true);
    ProgressBar(const ProgressBar &other);

    void update(std::ostream &out) const;

    constexpr uint64_t getMaximum() const { return uMax; }
    constexpr void setMaximum(uint64_t value) { uMax = value; }

    constexpr uint64_t getProgress() const { return uProgress; }
    constexpr void setProgress(uint64_t value) { uProgress = value; }

    constexpr const std::string &getMessage() const { return message; }
    constexpr void setMessage(const std::string &msg) { message = msg; }

    constexpr bool isComplete() const { return uProgress == uMax; }

    inline operator bool() { return this->isComplete(); }
    ProgressBar &operator++();
    ProgressBar &operator--();

    ProgressBar &operator=(const ProgressBar &);

    auto operator<=>(const ProgressBar &) const = default;

private:
    void writeTime(std::ostream &out, const std::chrono::duration<float> &dur) const;

private:
    std::string message;
    std::atomic<uint64_t> uMax = 100;
    std::atomic<uint64_t> uProgress = 0;
    bool bShowTime = false;
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
};
