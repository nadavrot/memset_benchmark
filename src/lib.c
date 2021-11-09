#include <stddef.h>

void *local_memset(void *s, int c, size_t n) {
  unsigned char *p = s;
  for (int i = 0; i < n; i++) {
    p[i] = c;
  }
  return s;
}
