#include "decl.h"

void *memset(void *s, int c, size_t n) { return local_memset(s, c, n); }