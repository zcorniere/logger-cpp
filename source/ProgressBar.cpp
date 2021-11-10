#include "ProgressBar.hpp"

#include <chrono>
#include <compare>
#include <cstdint>
#include <ratio>

struct TerminalSize {
    int columns = 0;
    int lines = 0;

    static TerminalSize get() noexcept;
};

#ifdef _WIN64
#include <windows.h>

TerminalSize TerminalSize::get() noexcept
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    TerminalSize ret;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    ret.columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    ret.lines = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return ret;
}

#elif __linux__

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

TerminalSize TerminalSize::get() noexcept
{
    TerminalSize ret;
    struct winsize w;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    ret.columns = w.ws_col;
    ret.lines = w.ws_row;
    return ret;
}

#endif

#define ESCAPE_SEQUENCE "\u001b"

ProgressBar::ProgressBar(std::string _message, unsigned max, bool show_time_)
    : data(new Data{
          .message = _message,
          .uMax = max,
          .bShowTime = show_time_,
      })
{
}

void ProgressBar::update(std::ostream &out) const
{
    const TerminalSize size = TerminalSize::get();
    const auto &[message, uMax, uProgress, bShowTime, start_time] = *data;

    unsigned uWidth = size.columns / 2;
    out << ESCAPE_SEQUENCE "[2K" ESCAPE_SEQUENCE "[1m" << message << ESCAPE_SEQUENCE "[0m\t[";
    unsigned fills = static_cast<float>(uProgress) / uMax * uWidth;
    for (unsigned i = 0; i < uWidth; i++) {
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
            auto elapsed = std::chrono::steady_clock::now() - start_time;
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
