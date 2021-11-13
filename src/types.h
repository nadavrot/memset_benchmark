#ifndef TYPES
#define TYPES

#include <stdint.h>

#define NO_INLINE __attribute__((noinline))

#ifdef __clang__
typedef char char8 __attribute__((ext_vector_type(8), aligned(1)));
typedef char char16 __attribute__((ext_vector_type(16), aligned(1)));
typedef char char32 __attribute__((ext_vector_type(32), aligned(1)));
typedef char char32a __attribute__((ext_vector_type(32), aligned(32)));

#else
// __GNUC__
typedef char char8 __attribute__((vector_size(8), aligned(1)));
typedef char char16 __attribute__((vector_size(16), aligned(1)));
typedef char char32 __attribute__((vector_size(32), aligned(1)));
typedef char char32a __attribute__((vector_size(32), aligned(32)));
#endif

typedef uint32_t __attribute__((aligned(1))) u32;
typedef uint64_t __attribute__((aligned(1))) u64;

#endif // TYPES
