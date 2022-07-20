#include "cpplogger/types/ProgressBar.hpp"

#include <cmath>
#include <iomanip>
#include <optional>
#include <ratio>
#include <string_view>
#include <utility>

#include "cpplogger/utils/Size.hpp"
#include "cpplogger/utils/sequences.hpp"

namespace cpplogger
{

ProgressBar::ProgressBar(std::string _message, unsigned max, Style style)
    : data{
          .message = std::move(_message),
          .uMax = max,
          .style = std::move(style),
      }
{
}

void ProgressBar::update(std::ostream &out) const
{
    const auto size = Size::get();

    const auto prefix_str = drawPrefix();

    std::string progress_str;
    if (data.style.bShowProgress) progress_str = drawProgress();

    std::string time_str;
    if (data.style.bShowTime) time_str = writeTime();

    std::string percentage;
    if (data.style.bShowPercentage) percentage = drawPercentage();

    const int iWidth = size.columns - int(2 + prefix_str.size() + progress_str.size() + time_str.size());

    out << prefix_str;
    if (iWidth > 0) { drawBar(out, iWidth); }
    if (!progress_str.empty()) out << " " << progress_str;
    if (!percentage.empty()) out << " " << percentage;
    if (!time_str.empty()) out << " " << time_str;
    out << std::endl;
}

std::string ProgressBar::writeTime() const
{
    static constexpr const auto remaining_text = " remaining";
    static constexpr const auto elapsed_text = " elapsed";

    const auto elapsed = std::chrono::steady_clock::now() - data.start_time;
    const auto remaining = elapsed / data.uProgress.load() * (data.uMax - data.uProgress);

    std::stringstream st;
    st.precision(1);
    if (data.uProgress > 0 && data.uProgress < data.uMax) {
        st << int(std::chrono::duration<float>(remaining).count()) << "s" << remaining_text << " | "
           << int(std::chrono::duration<float>(elapsed).count()) << "s" << elapsed_text;
    } else {
        st << " done";
    }
    return st.str();
}
void ProgressBar::drawBar(std::ostream &out, const int iWidth) const
{
    out << " [";
    const int fills = int(double(data.uProgress) / double(data.uMax) * iWidth);
    const auto &style = data.style;
    if (fills >= 0) {
        for (int i = 0; i < iWidth; i++) {
            if (i < fills) {
                out << style.cFill;
            } else if (i == fills) {
                out << style.cEqual;
            } else if (i > fills) {
                out << style.cEmpty;
            }
        }
    }
    out << "]";
}

std::string ProgressBar::drawProgress() const
{
    const int digit = std::floor(std::log10(double(data.uMax)) + 1);
    std::stringstream progress;
    progress << "(" << std::setfill(' ') << std::setw(digit) << data.uProgress << "/" << data.uMax << ")";
    return progress.str();
}

std::string ProgressBar::drawPrefix() const
{
    std::stringstream prefix;
    prefix << clearLine() << style(cpplogger::Style::Bold) << data.message << reset();
    return prefix.str();
}

std::string ProgressBar::drawPercentage() const
{
    const int fills = int((double(data.uProgress) / double(data.uMax)) * 100);
    return std::to_string(fills) + '%';
}

}    // namespace cpplogger