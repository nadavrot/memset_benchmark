#ifndef TIMER_UTILS
#define TIMER_UTILS

#include <chrono>
#include <iostream>
#include <string>

#include "types.h"

using time_point = std::chrono::steady_clock::time_point;

class TimerGuard {
  size_t div_;
  time_point begin_;

public:
  NO_INLINE
  TimerGuard(size_t div) : div_(div) {
    begin_ = std::chrono::steady_clock::now();
  }

  NO_INLINE
  ~TimerGuard() {
    time_point end = std::chrono::steady_clock::now();
    auto delta =
        std::chrono::duration_cast<std::chrono::microseconds>(end - begin_)
            .count();
    std::cout << uint64_t(double(delta) / div_) << ", ";
  }
};

#endif // TIMER_UTILS
