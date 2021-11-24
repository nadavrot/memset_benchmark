#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "decl.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////
// This is a small program that compares two memcpy implementations and records
// the output in a csv file.
////////////////////////////////////////////////////////////////////////////////

#define ITER (1000L * 1000L * 10L)
#define SAMPLES (20)

DoomRNG RNG;

/// Measure a single implementation \p handle.
uint64_t measure(memcpy_ty handle, void *dest, void *src, unsigned size) {
  Stopwatch T;
  for (unsigned i = 0; i < SAMPLES; i++) {
    T.start();
    for (size_t j = 0; j < ITER; j++) {
      (handle)(dest, src, size);
    }
    T.stop();
  }
  return T.get_median();
}

// Allocate memory and benchmark each implementation at a specific size \p size.
void bench_impl(const std::vector<memcpy_ty *> &toTest, unsigned size,
                unsigned align, unsigned offset) {
  std::vector<char> dest(size + 256, 0);
  std::vector<char> src(size + 256, 0);

  char *src_ptr = (char *)align_pointer(&src[0], align, offset);
  char *dest_ptr = (char *)align_pointer(&dest[0], align, offset);

  std::cout << size << ", ";
  for (auto handle : toTest) {
    u_int64_t res = measure(handle, dest_ptr, src_ptr, size);
    std::cout << res << ", ";
  }
  std::cout << std::endl;
}

int main(int argc, char **argv) {
  std::cout << std::setprecision(3);
  std::cout << std::fixed;

  std::vector<memcpy_ty *> toTest = {&memcpy, &__folly_memcpy, &local_memcpy};

  std::cout << "\nFixed size:\n";
  std::cout << "size, libc-direct, folly, naive_memcpy\n";

  for (int i = 0; i < 512; i++) {
    bench_impl(toTest, i, 16, 0);
  }

  return 0;
}
