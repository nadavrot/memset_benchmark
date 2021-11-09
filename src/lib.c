#include "types.h"

#include <stddef.h>

void *local_memset(void *s, int c, size_t n) {
  char *p = s;

  char X = c;
  char16 val16 = {X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X};

  while (n >= 16) {
    *((char16 *)p) = val16;
    p += 16;
    n -= 16;
  }

  for (int i = 0; i < n; i++) {
    p[i] = c;
  }
  return s;
}
