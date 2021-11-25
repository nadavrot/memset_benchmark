#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <chrono>
#include <string>

#include "types.h"

/// Aligns the pointer \p ptr, to alignment \p alignment and offset \p offset
/// within the word.
void *align_pointer(void *ptr, unsigned alignment, unsigned offset) {
  size_t p = (size_t)ptr;
  while (p % alignment)
    ++p;
  return (void *)(p + (size_t)offset);
}

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

uint8_t random_bytes[320] = {
    227, 138, 244, 198, 73,  247, 185, 248, 229, 75,  24,  215, 159, 230, 136,
    246, 200, 144, 65,  67,  109, 86,  118, 61,  209, 103, 188, 213, 187, 8,
    210, 121, 214, 178, 232, 59,  153, 92,  209, 239, 44,  85,  156, 172, 237,
    41,  150, 195, 247, 202, 249, 142, 208, 133, 21,  204, 114, 38,  51,  150,
    194, 46,  184, 138, 50,  250, 190, 180, 161, 5,   211, 191, 62,  137, 142,
    122, 63,  72,  233, 125, 189, 51,  238, 51,  116, 10,  44,  18,  240, 41,
    157, 81,  183, 252, 214, 17,  81,  12,  44,  119, 77,  97,  101, 80,  106,
    128, 190, 89,  160, 104, 244, 192, 46,  69,  73,  255, 45,  213, 190, 86,
    18,  89,  34,  46,  134, 145, 166, 128, 87,  97,  192, 71,  105, 94,  51,
    30,  7,   9,   0,   40,  0,   187, 205, 189, 151, 159, 107, 105, 180, 182,
    233, 52,  209, 108, 186, 31,  184, 254, 170, 71,  162, 31,  80,  226, 75,
    125, 214, 125, 247, 197, 149, 132, 247, 157, 253, 101, 107, 1,   127, 236,
    249, 242, 152, 169, 123, 240, 129, 230, 135, 25,  57,  227, 130, 189, 76,
    254, 33,  193, 39,  82,  177, 143, 31,  17,  20,  195, 219, 165, 171, 198,
    125, 119, 216, 143, 55,  210, 17,  88,  150, 126, 38,  160, 71,  214, 10,
    162, 158, 6,   234, 233, 119, 221, 167, 62,  146, 50,  150, 176, 142, 167,
    201, 250, 195, 26,  156, 96,  36,  177, 95,  23,  7,   63,  55,  142, 80,
    227, 73,  124, 93,  211, 231, 166, 182, 57,  145, 55,  242, 213, 246, 30,
    146, 247, 19,  229, 34,  210, 37,  147, 242, 103, 125, 91,  171, 51,  22,
    126, 248, 149, 19,  60,  89,  5,   241, 132, 72,  217, 195, 11,  173, 247,
    47,  144, 222, 94,  51,  166, 192, 50,  109, 62,  42,  126, 111, 204, 141,
    66,
};

/// Implements a doom-style random number generator.
struct DoomRNG {
  // Points to the current random number.
  unsigned rand_curr = 0;

  void rand_reset() { rand_curr = 0; }

  uint8_t next_u8_random() { return random_bytes[rand_curr++ % 320]; }
};

#endif // UTILS_H
