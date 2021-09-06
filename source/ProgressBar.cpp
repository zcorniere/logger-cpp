#include "ProgressBar.hpp"

#include <chrono>
#include <compare>
#include <cstdint>
#include <ratio>

#define ESCAPE_SEQUENCE "\u001b"

ProgressBar::ProgressBar(std::string _message, uint64_t max, bool show_time_)
    : data(std::make_shared<ProgressBar::Data>(_message, max, 0, show_time_))
{
}

void ProgressBar::update(std::ostream &out) const
{
    const auto &[message, uMax, uProgress, bShowTime, start_time] = *data;

    uint64_t uWidth = 40;
    out << ESCAPE_SEQUENCE "[2K" ESCAPE_SEQUENCE "[1m" << message << ESCAPE_SEQUENCE "[0m\t[";
    uint64_t fills = (int64_t)((float)uProgress / uMax * uWidth);
    for (uint64_t i = 0; i < uWidth; i++) {
        if (i < fills) {
            out << '=';
        } else if (i == fills) {
            out << '>';
        } else if (i > fills) {
            out << ' ';
        }
    }
    out << "] " << uProgress << '/' << uMax;

    if (bShowTime) {
        if (uProgress > 0 && uProgress < uMax) {
            out << ' ';
            auto elapsed = (std::chrono::steady_clock::now() - start_time);
            auto estimate = elapsed / uProgress.load() * (uMax - uProgress);
            writeTime(out, estimate);
            out << " remaining; ";
            writeTime(out, elapsed);
            out << " elapsed.";
        } else {
            out << " done";
        }
    }
    out << std::endl;
}

ProgressBar &ProgressBar::operator=(const ProgressBar &other) noexcept
{
    *this = other;
    return *this;
}

ProgressBar &ProgressBar::operator++() noexcept
{
    if (!this->isComplete()) data->uProgress++;
    return *this;
}

ProgressBar &ProgressBar::operator--() noexcept
{
    data->uProgress--;
    return *this;
}

void ProgressBar::writeTime(std::ostream &out, const std::chrono::duration<float> &dur) const
{
    using namespace std::chrono_literals;

    auto old_prec = out.precision();
    out.precision(3);
    if (dur > 1h) {
        out << std::chrono::duration<float, std::ratio<3600>>(dur).count() << "h";
    } else if (dur > 1min) {
        out << std::chrono::duration<float, std::ratio<60>>(dur).count() << "m";
    } else if (dur > 1s) {
        out << std::chrono::duration<float>(dur).count() << "s";
    } else {
        out << std::chrono::duration<float, std::milli>(dur).count() << "ms";
    }
    out.precision(old_prec);
}
