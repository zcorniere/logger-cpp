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
protected:
    struct Data {
        std::string message;
        std::atomic<unsigned> uMax = 100;
        std::atomic<unsigned> uProgress = 0;
        std::atomic<bool> bShowTime = false;
        std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    };

public:
    ProgressBar(std::string _message = "", unsigned max = 100, bool show_time_ = false);
    ~ProgressBar() = default;

    void update(std::ostream &out) const;

    [[nodiscard]] inline unsigned getMaximum() const noexcept { return data->uMax; }
    inline void setMaximum(unsigned value) noexcept { data->uMax = value; }

    [[nodiscard]] inline unsigned getProgress() const noexcept { return data->uProgress; }
    inline void setProgress(unsigned value) noexcept { data->uProgress = value; }

    [[nodiscard]] inline bool isShowingTime() const noexcept { return data->bShowTime; }
    inline void setShowTime(bool b) noexcept { data->bShowTime = b; }

    [[nodiscard]] inline const std::string &getMessage() const noexcept { return data->message; }
    inline void setMessage(const std::string &msg) noexcept { data->message = msg; }

    [[nodiscard]] inline bool isComplete() const noexcept { return data->uProgress >= data->uMax; }

    inline operator bool() const noexcept { return this->isComplete(); }

    ProgressBar &operator++() noexcept;
    ProgressBar &operator--() noexcept;

    std::strong_ordering operator<=>(const ProgressBar &) const noexcept = default;
    bool operator==(const ProgressBar &) const noexcept = default;

private:
    static std::string writeTime(const std::chrono::duration<float> &dur);
    void drawBar(std::ostream &out, const int uWidth) const;

private:
    std::shared_ptr<ProgressBar::Data> data;
};
