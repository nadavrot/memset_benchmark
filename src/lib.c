#include "utils.h"
#include <stddef.h>

void *local_memset(void *s, int c, size_t n) {
  char *p = s;

  /*
    if (n >= 16) {
      char X = c;
      // Stamp the vector stores in a loop:
      char16 val16 = {X,X,X,X,X,X,X,X,
                      X,X,X,X,X,X,X,X};

      for (int i = 0; i < (n - 15); i+=16) {
        *((char16*)(&p[i])) = val16;
      }

      if (n % 16 == 0) return s;
      // Stamp the last unaligned vector store.
      char* last = p + n - 16;
      (*(char16*)last) = val16;
      return s;
    }
  */
  for (int i = 0; i < n; i++) {
    p[i] = c;
  }
  return s;
}
