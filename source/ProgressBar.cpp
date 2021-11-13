#include "ProgressBar.hpp"

#include <chrono>
#include <compare>
#include <cstdint>
#include <ratio>
#include <sstream>

#include "Terminal.hpp"

ProgressBar::ProgressBar(std::string _message, unsigned max, bool show_time_)
    : data(new Data{
          .message = std::move(_message),
          .uMax = max,
          .bShowTime = show_time_,
      })
{
}

void ProgressBar::update(std::ostream &out) const
{
    static constexpr const std::string_view remaining_text = " remaining";
    static constexpr const std::string_view elapsed_text = " elapsed";
    static constexpr const auto timeSize = 10 + 10 + remaining_text.size() + elapsed_text.size();

    const Terminal::Size size = Terminal::Size::get();
    const auto &[message, uMax, uProgress, bShowTime, start_time] = *data;

    const auto elapsed = std::chrono::steady_clock::now() - start_time;
    const auto elapsed_str = writeTime(elapsed);
    const auto remaining = elapsed / uProgress.load() * (uMax - uProgress);
    const auto remaining_str = writeTime(remaining);

    int uWidth = size.columns - message.size() - 12 - ((bShowTime) ? (timeSize) : (0));

    out << ANSI_SEQUENCE(2, K) << Terminal::style<Terminal::Style::Bold> << message << Terminal::reset << "\t";

    if (uWidth > 0) drawBar(out, uWidth);

    out << "(" << uProgress << '/' << uMax << ")";

    if (bShowTime) {
        if (uProgress > 0 && uProgress < uMax) {
            out << ' ' << remaining_str << remaining_text << " | " << elapsed_str << elapsed_text;
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

std::string ProgressBar::writeTime(const std::chrono::duration<float> &dur)
{
    std::stringstream st;
    st.precision(1);

    st << int(std::chrono::duration<float>(dur).count()) << "s";
    return st.str();
}
void ProgressBar::drawBar(std::ostream &out, const int uWidth) const
{
    out << "[";
    int fills = static_cast<float>(data->uProgress) / data->uMax * uWidth;
    if (fills > 0) {
        for (auto i = 0; i < uWidth; i++) {
            if (i < fills) {
                out << '=';
            } else if (i == fills) {
                out << '>';
            } else if (i > fills) {
                out << ' ';
            }
        }
    }
    out << "] ";
}
