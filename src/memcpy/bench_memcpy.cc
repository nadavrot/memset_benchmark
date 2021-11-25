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

/// Allocate and copy buffers at random offsets and in random sizes.
/// The sizes and the offsets are in the range 0..256.
void bench_rand_range(const std::vector<memcpy_ty *> &toTest) {
  std::vector<char> dest(4096, 1);
  std::vector<char> src(4096, 0);
  const char *src_p = &src[0];
  char *dest_p = &dest[0];

  for (auto handle : toTest) {
    Stopwatch T;
    sleep(1);
    for (unsigned i = 0; i < SAMPLES; i++) {
      RNG.rand_reset();
      T.start();
      for (size_t j = 0; j < ITER; j++) {
        char *to = dest_p + RNG.next_u8_random();
        const char *from = src_p + RNG.next_u8_random();
        (handle)(to, from, RNG.next_u8_random());
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

  std::vector<memcpy_ty *> toTest = {&memcpy, &__folly_memcpy, &local_memcpy,
                                     &asm_memcpy};

  std::cout << "Batches of random sizes:\n";
  std::cout << "size, libc-direct, folly, c_memcpy, asm_memcpy\n";

  bench_rand_range(toTest);

  std::cout << "\nFixed size:\n";
  std::cout << "size, libc-direct, folly, c_memcpy, asm_memcpy\n";

  for (int i = 0; i < 512; i++) {
    bench_impl(toTest, i, 16, 0);
  }

  return 0;
}
