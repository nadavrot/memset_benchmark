#ifndef DECLS
#define DECLS

#include <stddef.h>

#ifdef __cplusplus

using memset_ty = void *(void *s, int c, size_t n);
using memcpy_ty = void *(void *dest, const void *src, size_t n);

extern "C" {
#endif

void *memcpy(void *dest, const void *src, size_t n);
void *__folly_memcpy(void *dest, const void *src, size_t n);
void *libc_memcpy(void *dest, const void *src, size_t n);
void *local_memcpy(void *dest, const void *src, size_t n);
void *asm_memcpy(void *dest, const void *src, size_t n);

void *memset(void *s, int c, size_t n);
void *libc_memset(void *s, int c, size_t n);
void *local_memset(void *s, int c, size_t n);
void *asm_memset(void *s, int c, size_t n);
void *musl_memset(void *s, int c, size_t n);

#ifdef __cplusplus
}
#endif

#endif // DECLS
