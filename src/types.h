#ifndef TYPES
#define TYPES

#define NO_INLINE __attribute__((noinline))

#ifdef __clang__
typedef char char8 __attribute__((ext_vector_type(8)));
typedef char char16 __attribute__((ext_vector_type(16)));
typedef char char32 __attribute__((ext_vector_type(32)));
#endif

#ifdef __GNUC__
typedef char char8 __attribute__((vector_size(8), aligned(1)));
typedef char char16 __attribute__((vector_size(16), aligned(1)));
typedef char char32 __attribute__((vector_size(32), aligned(1)));
#endif

#endif // TYPES
