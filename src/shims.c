#include "decl.h"

////////////////////////////////////////////////////////////////////////////////
/// This is a small utility that swaps the builtin call to memset with the
/// local implementation of memset, implemented in this project.
/// The shared object can be loaded using LD_PRELOAD (on Linux) or
/// DYLD_INSERT_LIBRARIES (on Mac).
////////////////////////////////////////////////////////////////////////////////

void *memset(void *s, int c, size_t n) { return local_memset(s, c, n); }
