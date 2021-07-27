#include "ProgressBar.hpp"
#include <chrono>

ProgressBar::ProgressBar(std::string _message, uint64_t max, bool show_time_)
    : uMax(max), message(_message), bShowTime(show_time_)
{
}

ProgressBar::~ProgressBar() {}

void ProgressBar::update(std::ostream &out) const
{
    uint64_t uWidth = 40;
    out << "\33[2K\033[1m" << message << "\033[0m\t[";
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
            auto estimate = elapsed / uProgress * (uMax - uProgress);
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

void ProgressBar::writeTime(std::ostream &out, std::chrono::duration<float> dur) const
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
