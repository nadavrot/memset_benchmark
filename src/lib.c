#include "types.h"

#include <stddef.h>
#include <stdint.h>

static inline void *small_memset(void *s, int c, size_t n) {
  if (n < 5) {
    if (n == 0)
      return s;
    char *p = s;
    p[0] = c;
    p[n - 1] = c;
    if (n <= 2)
      return s;
    p[1] = c;
    p[2] = c;
    return s;
  }

  if (n <= 16) {
    if (n <= 8) {
      uint32_t val4 = ((uint64_t)0x01010101L * ((uint8_t)c));
      char *first = s;
      char *last = s + n - 4;
      *((u32 *)first) = val4;
      *((u32 *)last) = val4;
      return s;
    }
    uint64_t val8 = ((uint64_t)0x0101010101010101L * ((uint8_t)c));
    char *first = s;
    char *last = s + n - 8;
    *((u64 *)first) = val8;
    *((u64 *)last) = val8;
    return s;
  }

  uint64_t val8 = ((uint64_t)0x0101010101010101L * ((uint8_t)c));
  char *first = s;
  char *second = s + n / 2 - 8;
  char *third = s + n / 2;
  char *fourth = s + n - 8;
  *((u64 *)first) = val8;
  *((u64 *)second) = val8;
  *((u64 *)third) = val8;
  *((u64 *)fourth) = val8;
  return s;
}

static inline void *huge_memset(void *s, int c, size_t n) {
  char *p = s;
  char X = c;
  char32 val32 = {X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
                  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X};

  // Stamp the first 32byte store.
  *((char32 *)p) = val32;

  char *first_aligned = p + 32 - ((uint64_t)p % 32);
  char *buffer_end = p + n;
  char *last_word = buffer_end - 32;

  // Align the next stores.
  p = first_aligned;

  // Unroll the body of the loop to increase parallelism.
  while (p + (32 * 5) < buffer_end) {
    *((char32 *)p) = val32;
    p += 32;
    *((char32 *)p) = val32;
    p += 32;
    *((char32 *)p) = val32;
    p += 32;
    *((char32 *)p) = val32;
    p += 32;
    *((char32 *)p) = val32;
    p += 32;
  }

  // Up to 3 iterations.
  while (p + 32 < buffer_end) {
    *((char32 *)p) = val32;
    p += 32;
  }

  // Stamp the last unaligned word.
  *((char32 *)last_word) = val32;
  return s;
}

void *local_memset(void *s, int c, size_t n) {
  char *p = s;
  char X = c;

  if (n <= 32) {
    return small_memset(s, c, n);
  }

  if (n > 200) {
    return huge_memset(s, c, n);
  }

  // n >= 32!
  char32 val32 = {X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
                  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X};

  // Stamp the last 32 bytes of the buffer.
  char *last = s + n - 32;

  // Stamp the first section of the buffer.
  while (n >= 32) {
    *((char32 *)p) = val32;
    p += 32;
    n -= 32;
  }

  *((char32 *)last) = val32;
  return s;
}

/// This a memset implementation that was copied from musl. We only use it for
/// benchmarking.

void *musl_memset(void *dest, int c, size_t n) {
  unsigned char *s = dest;
  size_t k;

  /* Fill head and tail with minimal branching. Each
   * conditional ensures that all the subsequently used
   * offsets are well-defined and in the dest region. */

  if (!n)
    return dest;
  s[0] = c;
  s[n - 1] = c;
  if (n <= 2)
    return dest;
  s[1] = c;
  s[2] = c;
  s[n - 2] = c;
  s[n - 3] = c;
  if (n <= 6)
    return dest;
  s[3] = c;
  s[n - 4] = c;
  if (n <= 8)
    return dest;

  /* Advance pointer to align it at a 4-byte boundary,
   * and truncate n to a multiple of 4. The previous code
   * already took care of any head/tail that get cut off
   * by the alignment. */

  k = -(uintptr_t)s & 3;
  s += k;
  n -= k;
  n &= -4;

#ifdef __GNUC__
  typedef uint32_t __attribute__((__may_alias__)) u32;
  typedef uint64_t __attribute__((__may_alias__)) u64;

  u32 c32 = ((u32)-1) / 255 * (unsigned char)c;

  /* In preparation to copy 32 bytes at a time, aligned on
   * an 8-byte bounary, fill head/tail up to 28 bytes each.
   * As in the initial byte-based head/tail fill, each
   * conditional below ensures that the subsequent offsets
   * are valid (e.g. !(n<=24) implies n>=28). */

  *(u32 *)(s + 0) = c32;
  *(u32 *)(s + n - 4) = c32;
  if (n <= 8)
    return dest;
  *(u32 *)(s + 4) = c32;
  *(u32 *)(s + 8) = c32;
  *(u32 *)(s + n - 12) = c32;
  *(u32 *)(s + n - 8) = c32;
  if (n <= 24)
    return dest;
  *(u32 *)(s + 12) = c32;
  *(u32 *)(s + 16) = c32;
  *(u32 *)(s + 20) = c32;
  *(u32 *)(s + 24) = c32;
  *(u32 *)(s + n - 28) = c32;
  *(u32 *)(s + n - 24) = c32;
  *(u32 *)(s + n - 20) = c32;
  *(u32 *)(s + n - 16) = c32;

  /* Align to a multiple of 8 so we can fill 64 bits at a time,
   * and avoid writing the same bytes twice as much as is
   * practical without introducing additional branching. */

  k = 24 + ((uintptr_t)s & 4);
  s += k;
  n -= k;

  /* If this loop is reached, 28 tail bytes have already been
   * filled, so any remainder when n drops below 32 can be
   * safely ignored. */

  u64 c64 = c32 | ((u64)c32 << 32);
  for (; n >= 32; n -= 32, s += 32) {
    *(u64 *)(s + 0) = c64;
    *(u64 *)(s + 8) = c64;
    *(u64 *)(s + 16) = c64;
    *(u64 *)(s + 24) = c64;
  }
#else
  /* Pure C fallback with no aliasing violations. */
  for (; n; n--, s++)
    *s = c;
#endif

  return dest;
}
