#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "decl.h"
#include "rand.h"
#include "timer_utils.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////
// This is a small program that compares two memset implementations and records
// the output in a csv file.
////////////////////////////////////////////////////////////////////////////////

#define ITER (1000L * 1000L * 10L)
#define SAMPLES (20)

/// Measure a single implementation \p handle.
uint64_t measure(memset_ty handle, unsigned size, unsigned align,
                 unsigned offset, void *ptr) {
  Stopwatch T;
  for (unsigned i = 0; i < SAMPLES; i++) {
    T.start();
    for (size_t j = 0; j < ITER; j++) {
      (handle)(ptr, 0, size);
    }
    T.stop();
  }
  return T.get_median();
}

// Allocate memory and benchmark each implementation at a specific size \p size.
void bench_impl(const std::vector<memset_ty *> &toTest, unsigned size,
                unsigned align, unsigned offset) {
  std::vector<char> memory(size + 256, 0);
  void *ptr = align_pointer(&memory[0], align, offset);

  std::cout << size << ", ";
  for (auto handle : toTest) {
    u_int64_t res = measure(handle, size, align, offset, ptr);
    std::cout << res << ", ";
  }
  std::cout << std::endl;
}

/// Try to allocate buffers at random offsets and in random sizes.
/// The sizes and the offsets are in the range 0..256.
void bench_rand_range(const std::vector<memset_ty *> &toTest) {
  std::vector<char> memory(1024, 0);
  void *ptr = &memory[0];

  for (auto handle : toTest) {
    Stopwatch T;

    for (unsigned i = 0; i < SAMPLES; i++) {
      rand_reset();
      T.start();
      for (size_t j = 0; j < ITER; j++) {
        (handle)((char *)ptr + next_u8_random(), 0, next_u8_random());
      }
      T.stop();
    }

    std::cout << T.get_median() << ", ";
  }
  std::cout << std::endl;
}

int main(int argc, char **argv) {
  std::cout << std::setprecision(3);
  std::cout << std::fixed;

  std::vector<memset_ty *> toTest = {musl_memset, libc_memset, &memset,
                                     local_memset, asm_memset};

  std::cout << "Batches of random sizes:\n";
  std::cout << " musl, libc@plt, libc-direct, c_memset, asm_memset\n";
  bench_rand_range(toTest);

  std::cout << "\nFixed size:\n";
  std::cout << "size, musl, libc@plt, libc-direct, c_memset, asm_memset\n";

  for (int i = 0; i < 512; i++) {
    bench_impl(toTest, i, 16, 0);
  }

  return 0;
}
