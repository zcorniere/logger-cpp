#include "ProgressBar.hpp"

#include <chrono>
#include <compare>
#include <cstdint>
#include <optional>
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
    const Terminal::Size size = Terminal::Size::get();
    const auto progress_str = drawProgress();

    std::optional<std::string> time_str = std::nullopt;
    const auto prefix_str = drawPrefix();

    if (data->bShowTime) time_str = writeTime();

    int uWidth = size.columns - (2 + prefix_str.size() + progress_str.size() + time_str.value_or("").size());

    out << prefix_str;
    if (uWidth > 0) { out << " " << drawBar(uWidth); }
    out << progress_str;
    if (time_str) { out << " " << *time_str; }
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

std::string ProgressBar::writeTime() const
{
    static constexpr const std::string_view remaining_text = " remaining";
    static constexpr const std::string_view elapsed_text = " elapsed";

    const auto &[message, uMax, uProgress, bShowTime, start_time] = *data;

    const auto elapsed = std::chrono::steady_clock::now() - start_time;
    const auto remaining = elapsed / uProgress.load() * (uMax - uProgress);

    std::stringstream st;
    st.precision(1);
    if (uProgress > 0 && uProgress < uMax) {
        st << int(std::chrono::duration<float>(elapsed).count()) << "s" << remaining_text << " | "
           << int(std::chrono::duration<float>(remaining).count()) << "s" << elapsed_text;
    } else {
        st << " done";
    }
    return st.str();
}
std::string ProgressBar::drawBar(const int uWidth) const
{
    std::stringstream out;
    out << "[";
    int fills = double(data->uProgress) / double(data->uMax) * uWidth;
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
    return out.str();
}

std::string ProgressBar::drawProgress() const
{
    std::stringstream progress;
    progress << "(" << data->uProgress << "/" << data->uMax << ")";
    return progress.str();
}

std::string ProgressBar::drawPrefix() const
{
    std::stringstream prefix;
    prefix << ANSI_SEQUENCE(2, K) << Terminal::style<Terminal::Style::Bold> << data->message << Terminal::reset;
    return prefix.str();
}
