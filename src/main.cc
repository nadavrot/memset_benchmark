#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

#include <unistd.h>

#include "decl.h"
#include "timer_utils.h"
#include "utils.h"

#define ITER (1000L * 1000L * 100L)
#define SAMPLES (10)

/// Generate a textual representation of the parameters.
std::string params_to_entry(const std::string &name, unsigned size,
                            unsigned alignment, unsigned offset) {
  return name + ",\t" + std::to_string(size) + ", " +
         std::to_string(alignment) + ", " + std::to_string(offset) + ", ";
}

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
void bench_impl(memset_ty handle0, memset_ty handle1, unsigned size,
                unsigned align, unsigned offset) {
  std::cout << size << ", " << align << ", " << offset << ", ";

  std::vector<char> memory(size + 256, 0);
  void *ptr = align_pointer(&memory[0], align, offset);
  uint64_t t0 = measure(handle0, SAMPLES, size, align, offset, ptr);
  uint64_t t1 = measure(handle1, SAMPLES, size, align, offset, ptr);

  std::cout << t0 << ", " << t1 << ", " << (double(t0) / (t1)) << ",";
}

int main(int argc, char **argv) {
  std::cout << std::fixed << "size, alignment, offset, libc, local\n";

  for (int i = 0; i < 512; i++) {
    bench_impl(libc_memset, local_memset, i, 16, 0);
    std::cout << "\n";
  }

  for (int sz2 = 0; sz2 < 10; sz2++) {
    bench_impl(libc_memset, local_memset, (8 << sz2), 16, 0);
    std::cout << "\n";
    bench_impl(libc_memset, local_memset, (8 << sz2) + 15, 16, 0);
    std::cout << "\n";
  }

  return 0;
}
