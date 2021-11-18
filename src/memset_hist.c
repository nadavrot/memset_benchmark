#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
/// This is a small utility that records calls to memset and creates a histogram
/// of the lengths of calls to memset. It prints the histogram when the program
/// is terminated. The shared object can be loaded using LD_PRELOAD (on Linux)
/// or DYLD_INSERT_LIBRARIES (on Mac).
////////////////////////////////////////////////////////////////////////////////

uint32_t counts[32] = { 0, };

const int tab32[32] = {0,  9,  1,  10, 13, 21, 2,  29, 11, 14, 16,
                       18, 22, 25, 3,  30, 8,  12, 20, 28, 15, 17,
                       24, 7,  19, 27, 23, 6,  26, 5,  4,  31};

int log2_32(uint32_t value) {
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  return tab32[(uint32_t)(value * 0x07C4ACDD) >> 27];
}

void __attribute__((destructor)) print_hitogram() {
  printf("Memset histogram:\n");
  int i;
  for (i = 0; i < 30; i++) {
    fprintf(stderr, "%d, %d\n", i, counts[i]);
  }
}

void *memset(void *s, int c, size_t len) {
  counts[log2_32(len)]++;
  char *p = s;

  for (int i = 0; i < len; i++) {
    p[i] = c;
  }
  return s;
}

