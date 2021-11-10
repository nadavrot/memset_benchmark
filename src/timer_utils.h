#ifndef TIMER_UTILS
#define TIMER_UTILS

#include <chrono>
#include <iostream>
#include <string>

#include "types.h"

using time_point = std::chrono::steady_clock::time_point;

class Stopwatch {
  time_point begin_;

public:
  NO_INLINE
  Stopwatch() : begin_(std::chrono::steady_clock::now()) {}

  /// \returns the time delta microseconds.
  NO_INLINE
  uint64_t getTimeDelta() {
    time_point end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - begin_)
        .count();
  }
};

#endif // TIMER_UTILS
