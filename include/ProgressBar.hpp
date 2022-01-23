#pragma once

#include <atomic>
#include <chrono>
#include <compare>
#include <memory>
#include <ostream>
#include <stdint.h>
#include <string>

namespace details
{
struct Style {
    const char cFill = '=';
    const char cEqual = '>';
    const char cEmpty = ' ';
    const bool bShowTime = false;
};
};    // namespace details

class ProgressBar
{
public:
    // Can't have Style as a sub-class, because of this:
    // https://bugs.llvm.org/show_bug.cgi?id=36684
    using Style = details::Style;

protected:
    struct Data {
        std::string message;
        std::atomic<unsigned> uMax = 100;
        std::atomic<unsigned> uProgress = 0;
        std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
        const Style style = {};
    };

public:
    explicit ProgressBar(std::string _message = "", unsigned max = 100, const Style style = Style());
    ~ProgressBar() = default;

    void update(std::ostream &out) const;

    [[nodiscard]] inline unsigned getMaximum() const noexcept { return data->uMax; }
    inline void setMaximum(unsigned value) noexcept { data->uMax = value; }

    [[nodiscard]] inline unsigned getProgress() const noexcept { return data->uProgress; }
    inline void setProgress(unsigned value) noexcept { data->uProgress = value; }

    [[nodiscard]] inline const std::string &getMessage() const noexcept { return data->message; }
    inline void setMessage(const std::string &msg) noexcept { data->message = msg; }

    [[nodiscard]] inline bool isComplete() const noexcept { return data->uProgress >= data->uMax; }

    inline operator bool() const noexcept { return this->isComplete(); }

    ProgressBar &operator++() noexcept;
    ProgressBar &operator--() noexcept;

    std::strong_ordering operator<=>(const ProgressBar &) const noexcept = default;
    bool operator==(const ProgressBar &) const noexcept = default;

private:
    std::string writeTime() const;
    std::string drawBar(const size_t uWidth) const;
    std::string drawProgress() const;
    std::string drawPrefix() const;

private:
    std::shared_ptr<ProgressBar::Data> data;
};
