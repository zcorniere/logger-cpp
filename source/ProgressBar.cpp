#include "ProgressBar.hpp"

#include <assert.h>
#include <chrono>
#include <optional>
#include <ratio>
#include <sstream>
#include <string_view>
#include <utility>

#include "Size.hpp"
#include "utils/sequences.hpp"

namespace cpplogger
{

ProgressBar::ProgressBar(std::string _message, unsigned max, Style style)
    : data(new Data{
          .message = std::move(_message),
          .uMax = max,
          .style = std::move(style),
      })
{
    assert(max > 0);
}

void ProgressBar::update(std::ostream &out) const
{
    const auto size = Size::get();
    const auto progress_str = drawProgress();

    std::optional<std::string> time_str = std::nullopt;
    const auto prefix_str = drawPrefix();

    if (data->style.bShowTime) time_str = writeTime();

    const int iWidth = size.columns - int(2 + prefix_str.size() + progress_str.size() + time_str.value_or("").size());

    out << prefix_str;
    if (iWidth > 0) { out << " " << drawBar(iWidth); }
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

    const auto elapsed = std::chrono::steady_clock::now() - data->start_time;
    const auto remaining = elapsed / data->uProgress.load() * (data->uMax - data->uProgress);

    std::stringstream st;
    st.precision(1);
    if (data->uProgress > 0 && data->uProgress < data->uMax) {
        st << int(std::chrono::duration<float>(elapsed).count()) << "s" << remaining_text << " | "
           << int(std::chrono::duration<float>(remaining).count()) << "s" << elapsed_text;
    } else {
        st << " done";
    }
    return st.str();
}
std::string ProgressBar::drawBar(const int iWidth) const
{
    std::stringstream out;
    out << "[";
    const int fills = int(double(data->uProgress) / double(data->uMax) * iWidth);
    if (fills >= 0) {
        for (int i = 0; i < iWidth; i++) {
            if (i < fills) {
                out << data->style.cFill;
            } else if (i == fills) {
                out << data->style.cEqual;
            } else if (i > fills) {
                out << data->style.cEmpty;
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
    prefix << clearLine() << style(cpplogger::Style::Bold) << data->message << reset();
    return prefix.str();
}

}    // namespace cpplogger