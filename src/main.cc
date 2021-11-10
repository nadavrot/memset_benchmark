#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

#include "timer_utils.h"
#include "utils.h"

#define ITER (1000 * 1000 * 1000)

extern "C" {
void *memset(void *s, int c, size_t n);
void *libc_memset(void *s, int c, size_t n);
void *local_memset(void *s, int c, size_t n);
void *musl_memset(void *s, int c, size_t n);
}

using memset_ty = void *(void *s, int c, size_t n);

/// Generate a textual representation of the parameters.
std::string params_to_entry(const std::string &name, unsigned size,
                            unsigned alignment, unsigned offset) {
  return name + ",\t" + std::to_string(size) + ", " +
         std::to_string(alignment) + ", " + std::to_string(offset) + ", ";
}

// Allocate memory and benchmark a single implementation.
void bench_impl(memset_ty handle0, memset_ty handle1, unsigned SIZE,
                unsigned ALIGN, unsigned OFFSET) {
  std::cout << SIZE << ", " << ALIGN << ", " << OFFSET << ", ";

  std::vector<char> memory(SIZE + 256, 0);
  void *ptr = align_pointer(&memory[0], ALIGN, OFFSET);

  uint64_t t0, t1;

  {
    Stopwatch T;
    for (int i = 0; i < ITER; i++) {
      (handle0)(ptr, 0, SIZE);
    }
    t0 = T.getTimeDelta();
  }

  {
    Stopwatch T;
    for (int i = 0; i < ITER; i++) {
      (handle1)(ptr, 0, SIZE);
    }
    t1 = T.getTimeDelta();
  }
  std::cout << t0 << ", " << t1 << ", " << (double(t0) / (t1)) << ",";
}

int main(int argc, char **argv) {
  std::cout << std::fixed << "Size, Alignment, Offset, libc , local\n";

  for (int i = 1; i < 16; i++) {
    bench_impl(libc_memset, local_memset, i, 16, 0);
    std::cout << "\n";
  }

  for (int sz2 = 0; sz2 < 10; sz2++) {
    bench_impl(libc_memset, local_memset, 8 << sz2, 16, 0);
    std::cout << "\n";
  }

  return 0;
}
