#pragma once

#include <atomic>
#include <chrono>
#include <ostream>
#include <stdint.h>
#include <string>

class ProgressBar
{
public:
    ProgressBar(std::string _message = "", uint64_t max = 100, bool show_time_ = false);
    ProgressBar(const ProgressBar &other);

    void update(std::ostream &out) const;

    [[nodiscard]] inline uint64_t getMaximum() const { return uMax; }
    inline void setMaximum(uint64_t value) { uMax = value; }

    [[nodiscard]] inline uint64_t getProgress() const { return uProgress; }
    inline void setProgress(uint64_t value) { uProgress = value; }

    [[nodiscard]] inline bool isShowingTime() const { return bShowTime; }
    inline void setShowTime(bool b) { bShowTime = b; }

    [[nodiscard]] inline const std::string &getMessage() const { return message; }
    inline void setMessage(const std::string &msg) { message = msg; }

    [[nodiscard]] inline bool isComplete() const { return uProgress == uMax; }

    inline operator bool() { return this->isComplete(); }

    ProgressBar &operator++();
    ProgressBar &operator--();

    ProgressBar &operator=(const ProgressBar &);

    std::strong_ordering operator<=>(const ProgressBar &) const = default;
    bool operator==(const ProgressBar &) const = default;

private:
    void writeTime(std::ostream &out, const std::chrono::duration<float> &dur) const;

private:
    std::string message;
    std::atomic<uint64_t> uMax = 100;
    std::atomic<uint64_t> uProgress = 0;
    std::atomic<bool> bShowTime = false;
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
};
