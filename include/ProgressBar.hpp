#pragma once

#include <chrono>
#include <ostream>
#include <stdint.h>
#include <string>

class ProgressBar
{
public:
    ProgressBar(std::string _message = "", uint64_t max = 100, bool show_time_ = true);
    ~ProgressBar();

    void update(std::ostream &out) const;

    uint64_t getMaximum() const;
    void setMaximum(uint64_t value);

    uint64_t getProgress() const;
    void setProgress(uint64_t value);

    ProgressBar &operator++();
    ProgressBar &operator--();

    auto operator<=>(const ProgressBar &) const = default;

private:
    void writeTime(std::ostream &out, std::chrono::duration<float> dur) const;

private:
    uint64_t uMax = 100;
    uint64_t uProgress = 0;
    std::string message;
    bool bShowTime = false;
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
};
