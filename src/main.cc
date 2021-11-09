#include <cstring>
#include <iostream>
#include <vector>

#include "timer_utils.h"

#define ITER (1000 * 1000 * 1000)

extern "C" {
void *memset(void *s, int c, size_t n);
void *libc_memset(void *s, int c, size_t n);
void *local_memset(void *s, int c, size_t n);
}

using memset_ty = void *(void *s, int c, size_t n);

/// Alligns the pointer \p ptr, to alignment \p alignment and offset \p offset
/// within the word.
void *align_pointer(void *ptr,unsigned alignment, unsigned offset) {
  size_t p = (size_t)ptr;
  while (p % alignment)
    ++p;
  return (void *)(p + (size_t)offset);
}

/// Generate a textual representation of the parameters.
std::string params_to_entry(const std::string &name, unsigned size,
                            unsigned alignment, unsigned offset) {
  return name + ",\t" + std::to_string(size) + ", " +
         std::to_string(alignment) + ", " + std::to_string(offset) + ", ";
}

// Allocate memory and benchmark a single implementation.
void bench_impl(memset_ty handle, const std::string &name, unsigned SIZE,
                unsigned ALIGN, unsigned OFFSET) {
  TimerGuard T(params_to_entry(name, SIZE, ALIGN, OFFSET), SIZE);
  std::vector<char> memory(SIZE + 256, 0);
  void *ptr = align_pointer(&memory[0], ALIGN, OFFSET);

  for (int i = 0; i < ITER; i++) {
    (handle)(ptr, 0, SIZE);
  }
}

/*
// Allocate memory and benchmark a single implementation.
void posix(unsigned SIZE, unsigned ALIGN, unsigned OFFSET) {
  TimerGuard T(params_to_entry("posix", SIZE, ALIGN, OFFSET), SIZE);
  std::vector<char> memory(SIZE + 256, 0);
  void *ptr = align_pointer(&memory[0], ALIGN, OFFSET);

  for (int i = 0; i < ITER; i++) {
    libc_memset(ptr, 0, SIZE);
  }
}
*/
int main(int argc, char **argv) {
  std::cout<<"Name, size, alignment, offset,\n";

#define BENCH(FUNC, SIZE, ALIGN, OFFSET)                                       \
  bench_impl(FUNC, #FUNC, SIZE, ALIGN, OFFSET);

  for (int sz2 = 0; sz2 < 10; sz2++) {
    BENCH(libc_memset, 8 << sz2, 16, 0);
    BENCH(local_memset, 8 << sz2, 16, 0);
  }

  return 0;
}
