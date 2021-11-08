#ifndef TIMER_UTILS
#define TIMER_UTILS

#include <chrono>
#include <iostream>
#include <string>

#include "utils.h"

using time_point = std::chrono::steady_clock::time_point;

class TimerGuard {
  std::string msg_;
  time_point begin_;

public:
  NO_INLINE
  TimerGuard(const std::string &msg) : msg_(msg) {
    begin_ = std::chrono::steady_clock::now();
  }

  NO_INLINE
  ~TimerGuard() {
    time_point end = std::chrono::steady_clock::now();
    auto delta =
        std::chrono::duration_cast<std::chrono::microseconds>(end - begin_)
            .count();
    std::cout << msg_ << " " << delta << "\n";
  }
};

#endif // TIMER_UTILS
