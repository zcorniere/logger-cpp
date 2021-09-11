#pragma once

#include <atomic>
#include <chrono>
#include <compare>
#include <memory>
#include <ostream>
#include <stdint.h>
#include <string>

class ProgressBar
{
public:
    enum Status {
        New,
        Ok,
        Delete,
    };

protected:
    struct Data {
        std::string message;
        std::atomic<uint64_t> uMax = 100;
        std::atomic<uint64_t> uProgress = 0;
        std::atomic<bool> bShowTime = false;
        std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    };

public:
    ProgressBar(std::string _message = "", uint64_t max = 100, bool show_time_ = false);
    ~ProgressBar() = default;

    void update(std::ostream &out) const;

    [[nodiscard]] inline uint64_t getMaximum() const noexcept { return data->uMax; }
    inline void setMaximum(uint64_t value) noexcept { data->uMax = value; }

    [[nodiscard]] inline uint64_t getProgress() const noexcept { return data->uProgress; }
    inline void setProgress(uint64_t value) noexcept { data->uProgress = value; }

    [[nodiscard]] inline bool isShowingTime() const noexcept { return data->bShowTime; }
    inline void setShowTime(bool b) noexcept { data->bShowTime = b; }

    [[nodiscard]] inline const std::string &getMessage() const noexcept { return data->message; }
    inline void setMessage(const std::string &msg) noexcept { data->message = msg; }

    [[nodiscard]] inline bool isComplete() const noexcept { return data->uProgress == data->uMax; }

    inline operator bool() noexcept { return this->isComplete(); }

    ProgressBar &operator++() noexcept;
    ProgressBar &operator--() noexcept;

    std::strong_ordering operator<=>(const ProgressBar &) const noexcept = default;
    bool operator==(const ProgressBar &) const noexcept = default;

private:
    void writeTime(std::ostream &out, const std::chrono::duration<float> &dur) const;

private:
    std::shared_ptr<ProgressBar::Data> data;
};
