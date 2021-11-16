#if defined(__APPLE__)
.text
.global _libc_memset
.p2align  4, 0x90
_libc_memset:
        _jmp memset

#else

.text
.global libc_memset
.p2align  4, 0x90
libc_memset:
        jmp memset
#endif