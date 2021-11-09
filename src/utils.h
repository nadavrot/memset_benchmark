#ifndef UTILS
#define UTILS

/// Aligns the pointer \p ptr, to alignment \p alignment and offset \p offset
/// within the word.
void *align_pointer(void *ptr, unsigned alignment, unsigned offset) {
  size_t p = (size_t)ptr;
  while (p % alignment)
    ++p;
  return (void *)(p + (size_t)offset);
}

#endif // UTILS
