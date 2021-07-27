#pragma once

#include <chrono>
#include <ostream>

class ProgressBar {
public:
  ProgressBar(std::string _message = "", uint64_t max = 100,
              bool show_time_ = true);
  ~ProgressBar();

  void update(std::ostream &out) const;

  uint64_t getMaximum() const { return uMax; }
  void setMaximum(uint64_t value) { uMax = value; }

  uint64_t getProgress() const { return uProgress; }
  void setProgress(uint64_t value) { uProgress = value; }

  ProgressBar &operator++() {
    uProgress++;
    return *this;
  }
  ProgressBar &operator--() {
    uProgress--;
    return *this;
  }

  auto operator<=>(const ProgressBar &) const = default;

private:
  void writeTime(std::ostream &out, std::chrono::duration<float> dur) const;

private:
  uint64_t uMax = 100;
  uint64_t uProgress = 0;
  std::string message;
  bool bShowTime = false;
  std::chrono::steady_clock::time_point start_time =
      std::chrono::steady_clock::now();
};
