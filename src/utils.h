#ifndef UTILS
#define UTILS

#define NO_INLINE __attribute__((noinline))

#ifdef __clang__
typedef char char16 __attribute__((ext_vector_type(16)));
typedef char char32 __attribute__((ext_vector_type(32)));

#endif

#ifdef __GNUC__
typedef char char16 __attribute__((vector_size(16)));
typedef char char32 __attribute__((vector_size(32)));
#endif

#endif // UTILS
