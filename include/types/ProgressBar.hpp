#pragma once

#include <atomic>
#include <chrono>
#include <compare>
#include <ostream>
#include <string>

#include "interface/IUpdate.hpp"

namespace cpplogger
{

namespace details
{
    struct ProgressBarStyle {
        const char cFill = '=';
        const char cEqual = '>';
        const char cEmpty = ' ';
        const bool bShowTime = false;
        const bool bShowPercentage = false;
        const bool bShowProgress = true;
        bool operator==(const ProgressBarStyle &) const = default;
    };
};    // namespace details

class ProgressBar final : public IUpdate
{
public:
    // Can't have Style as a sub-class, because of this:
    // https://bugs.llvm.org/show_bug.cgi?id=36684
    using Style = details::ProgressBarStyle;

protected:
    struct Data {
        std::string message;
        std::atomic<unsigned> uMax = 100;
        std::atomic<unsigned> uProgress = 0;
        std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
        const Style style = {};
        bool operator==(const Data &) const = default;
    };

public:
    explicit ProgressBar(std::string _message = "", unsigned max = 100, const Style style = Style());
    ~ProgressBar() = default;

    void update(std::ostream &out) const override;

    [[nodiscard]] unsigned getMaximum() const noexcept { return data.uMax; }
    void setMaximum(unsigned value) noexcept { data.uMax = value; }

    [[nodiscard]] unsigned getProgress() const noexcept { return data.uProgress; }
    void setProgress(unsigned value) noexcept { data.uProgress = value; }
    void addProgress(unsigned increment) noexcept { data.uProgress += increment; }

    [[nodiscard]] const std::string &getMessage() const noexcept { return data.message; }
    void setMessage(const std::string &msg) noexcept { data.message = msg; }

    bool isComplete() const noexcept override { return data.uProgress >= data.uMax; }

    operator bool() const noexcept { return this->isComplete(); }
    bool operator==(const IUpdate &obj) const
    {
        const auto *bar = dynamic_cast<const ProgressBar *>(&obj);
        if (bar == nullptr) return false;
        return data == bar->data;
    };

private:
    std::string writeTime() const;
    void drawBar(std::ostream &out, const int uWidth) const;
    std::string drawProgress() const;
    std::string drawPrefix() const;
    std::string drawPercentage() const;

private:
    ProgressBar::Data data;
};

}    // namespace cpplogger