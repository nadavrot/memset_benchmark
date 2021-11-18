#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "decl.h"
#include "timer_utils.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////
// This is a small program that compares two memset implementations and records
// the output in a csv file.
////////////////////////////////////////////////////////////////////////////////

#define ITER (1000L * 1000L * 10L)
#define SAMPLES (30)

uint64_t measure(memset_ty handle, unsigned samples, unsigned size,
                 unsigned align, unsigned offset, void *ptr) {
  std::vector<uint64_t> tv;
  for (unsigned i = 0; i < samples; i++) {
    Stopwatch T;
    for (size_t i = 0; i < ITER; i++) {
      (handle)(ptr, 0, size);
    }
    tv.push_back(T.getTimeDelta());
  }
  std::sort(tv.begin(), tv.end());
  // Return the median of the samples.
  return tv[tv.size() / 2];
}

// Allocate memory and benchmark a single implementation.
void bench_impl(const std::vector<memset_ty *> &toTest, unsigned size,
                unsigned align, unsigned offset) {
  std::vector<char> memory(size + 256, 0);
  void *ptr = align_pointer(&memory[0], align, offset);

  std::cout << size << ", ";
  for (auto handle : toTest) {
    u_int64_t res = measure(handle, SAMPLES, size, align, offset, ptr);
    std::cout << res << ", ";
  }
  std::cout << std::endl;
}

int main(int argc, char **argv) {
  std::cout << std::setprecision(3);
  std::cout << std::fixed
            << "size, musl, libc@plt, libc-direct, c_memset, asm_memset\n";

  std::vector<memset_ty *> toTest = {musl_memset, libc_memset, &memset,
                                     local_memset, asm_memset};

  for (int i = 0; i < 512; i++) {
    bench_impl(toTest, i, 16, 0);
  }

  return 0;
}
