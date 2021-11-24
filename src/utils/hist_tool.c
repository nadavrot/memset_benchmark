#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
/// This is a small utility that records calls to some methods and creates a
/// histogram of the lengths of calls to memset. It prints the histogram when
/// the program is terminated. The shared object can be loaded using LD_PRELOAD
/// (on Linux) or DYLD_INSERT_LIBRARIES (on Mac).
////////////////////////////////////////////////////////////////////////////////

uint32_t memset_len_dist[32] = {
    0,
};
uint32_t memcpy_len_dist[32] = {
    0,
};

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

void __attribute__((destructor)) print_hitograms() {
  FILE *ff = fopen("/tmp/hist.txt", "a+");
  if (!ff) {
    return;
  }
  pid_t pid = getpid();

  fprintf(ff, "Histogram for (%d):\n", pid);
  fprintf(ff, "size, memset, memcpy:\n");
  for (int i = 0; i < 32; i++) {
    fprintf(ff, "%d, %d, %d,\n", i, memset_len_dist[i], memcpy_len_dist[i]);
  }
  fclose(ff);
}

void *memcpy(void *dest, const void *src, size_t len) {
  memcpy_len_dist[log2_32(len)]++;
  char *d = (char *)dest;
  char *s = (char *)src;
  for (size_t i = 0; i < len; i++) {
    d[i] = s[i];
  }
  return dest;
}

void *memset(void *s, int c, size_t len) {
  memset_len_dist[log2_32(len)]++;
  char *p = s;

  for (int i = 0; i < len; i++) {
    p[i] = c;
  }
  return s;
}

