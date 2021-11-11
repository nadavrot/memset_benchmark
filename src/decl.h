#ifndef DECLS
#define DECLS

#include <stddef.h>

#ifdef __cplusplus

using memset_ty = void *(void *s, int c, size_t n);

extern "C" {
#endif

void *memset(void *s, int c, size_t n);
void *libc_memset(void *s, int c, size_t n);
void *local_memset(void *s, int c, size_t n);
void *musl_memset(void *s, int c, size_t n);

#ifdef __cplusplus
}
#endif

#endif // DECLS
