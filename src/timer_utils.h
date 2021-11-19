#ifndef TIMER_UTILS
#define TIMER_UTILS

#include <algorithm>
#include <chrono>
#include <string>

#include "types.h"

using time_point = std::chrono::steady_clock::time_point;

class Stopwatch {
  /// The time of the last sample;
  time_point begin_;
  /// A list of recorded intervals.
  std::vector<uint64_t> intervals_;

public:
  NO_INLINE
  Stopwatch() : begin_() {}

  NO_INLINE
  void start() { begin_ = std::chrono::steady_clock::now(); }

  NO_INLINE
  void stop() {
    time_point end = std::chrono::steady_clock::now();
    uint64_t interval =
        std::chrono::duration_cast<std::chrono::microseconds>(end - begin_)
            .count();
    intervals_.push_back(interval);
  }

  NO_INLINE
  uint64_t get_median() {
    std::sort(intervals_.begin(), intervals_.end());
    return intervals_[intervals_.size() / 2];
  }
};

#endif // TIMER_UTILS
