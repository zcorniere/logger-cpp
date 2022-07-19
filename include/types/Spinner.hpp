#pragma once

#include "interface/IUpdate.hpp"

#include <atomic>
#include <vector>

namespace cpplogger
{

namespace details
{
    struct SpinnerStyle {
        const std::vector<std::string> style = {"▖", "▘", "▝", "▗"};
        const std::string separator = " ";
        const std::string completion = "✔";
        bool operator==(const SpinnerStyle &) const = default;
    };
}    // namespace details

class Spinner : public IUpdate
{
public:
    using Style = details::SpinnerStyle;

public:
    Spinner(const std::string &message, const Style = {});
    void update(std::ostream &out) const override;

    bool isComplete() const noexcept override { return bIsComplete; };
    void setComplete(bool bCompletion = true) noexcept { bIsComplete = bCompletion; }
    bool operator==(const IUpdate &obj) const
    {
        const auto *bar = dynamic_cast<const Spinner *>(&obj);
        if (bar == nullptr) return false;
        return style == bar->style;
    };

private:
    mutable std::size_t index = 0;
    std::atomic<bool> bIsComplete = false;
    std::string message;
    const Style style{};
};

}    // namespace cpplogger