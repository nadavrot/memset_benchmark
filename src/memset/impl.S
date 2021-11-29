#if defined(__APPLE__)
.text
.global _libc_memset
.p2align  4, 0x90
_libc_memset:
        jmp _memset

#else

.text
.global libc_memset
.p2align  4, 0x90
libc_memset:
        jmp memset
#endif

#define LABEL(x)     .L##x
#if defined(__APPLE__)
.text
.global _asm_memset
.p2align  5, 0x90
_asm_memset:
#else
.text
.global asm_memset
.p2align  5, 0x90
asm_memset:
#endif

// RDI is the buffer
// RSI is the value
// RDX is length
        vmovd  %esi, %xmm0
        vpbroadcastb %xmm0,%ymm0
        mov    %rdi,%rax
        cmp    $0x40,%rdx
        jae LABEL(above_64)
LABEL(below_64):
        cmp    $0x20, %rdx
        jb LABEL(below_32)
        vmovdqu %ymm0,(%rdi)
        vmovdqu %ymm0,-0x20(%rdi,%rdx)
        vzeroupper
        retq
LABEL(below_32):
        cmp    $0x10, %rdx
        jae     LABEL(in_16_to_32)
LABEL(below_16):
        cmp    $0x4, %rdx
        jbe     LABEL(below_4)
LABEL(in_4_to_16):
        // Scalar stores from this point.
        vmovq %xmm0, %rsi
        cmp    $0x7, %rdx
        jbe    LABEL(in_4_to_8)
        // two 8-wide stores, up to 16 bytes.
        mov    %rsi, -0x8(%rdi, %rdx)
        mov    %rsi,(%rdi)
        vzeroupper
        retq
.align 4
LABEL(below_4):
        test   %rdx, %rdx
        je     LABEL(exit)
        mov    %sil, (%rdi)
        mov    %sil, -0x1(%rdi,%rdx)
        cmp    $0x2, %rdx
        jbe    LABEL(exit)
        mov     %sil, 0x1(%rdi)
        mov     %sil, 0x2(%rdi)
        mov    %rdi,%rax
.align 4
LABEL(exit):
        vzeroupper
        retq
LABEL(in_4_to_8):
        // two 4-wide stores, upto 8 bytes.
        mov    %esi,-0x4(%rdi,%rdx)
        mov    %esi,(%rdi)
        vzeroupper
        retq
LABEL(in_16_to_32):
        vmovups %xmm0,(%rdi)
        vmovups %xmm0,-0x10(%rdi,%rdx)
        vzeroupper
        retq
LABEL(above_64):
        cmp    $0xb0, %rdx
        ja LABEL(above_192)
        cmp    $0x80, %rdx
        jbe LABEL(in_64_to_128)
        // Do some work filling unaligned 32bit words.
        // last_word -> rsi
        lea    -0x20(%rdi,%rdx),%rsi
        // rcx -> fill pointer.

        // We have at least 128 bytes to store.
        vmovdqu %ymm0,(%rdi)
        vmovdqu %ymm0, 0x20(%rdi)
        vmovdqu %ymm0, 0x40(%rdi)
        add    $0x60,%rdi
.align 8
LABEL(fill_32):
        vmovdqu %ymm0,(%rdi)
        add    $0x20,%rdi
        cmp    %rdi,%rsi
        ja     LABEL(fill_32)
        // Stamp the last unaligned store.
        vmovdqu %ymm0,(%rsi)
        vzeroupper
        retq
LABEL(in_64_to_128):
        // last_word -> rsi
        vmovdqu %ymm0,(%rdi)
        vmovdqu %ymm0, 0x20(%rdi)
        vmovdqu %ymm0,-0x40(%rdi,%rdx)
        vmovdqu %ymm0,-0x20(%rdi,%rdx)
        vzeroupper
        retq

LABEL(above_192):
// rdi is the buffer address
// rsi is the value
// rdx is length
        // Store the first unaligned 32 bytes.
        vmovdqu %ymm0,(%rdi)

        // The first aligned word is stored in %rsi.
        mov    %rdi,%rsi
        and    $0xffffffffffffffe0,%rsi
        lea    0x20(%rsi),%rsi

        // Compute the address of the last unaligned word into rdi.
        lea    -0x20(%rdx), %rdx
        add     %rdx, %rdi

        // Check if we can do a full 5x32B stamp.
        lea    0xa0(%rsi),%rcx
        cmp    %rcx, %rdi
        jb     LABEL(stamp_4)
.align 8
LABEL(fill_192):
        vmovdqa %ymm0,(%rsi)
        vmovdqa %ymm0,0x20(%rsi)
        vmovdqa %ymm0,0x40(%rsi)
        vmovdqa %ymm0,0x60(%rsi)
        vmovdqa %ymm0,0x80(%rsi)
        add     $0xa0, %rsi
        lea    0xa0(%rsi),%rcx
        cmp    %rcx, %rdi
        ja     LABEL(fill_192)

LABEL(fill_192_tail):
        cmp    %rsi, %rdi
        jb     LABEL(fill_192_done)
        vmovdqa %ymm0, (%rsi)

        lea    0x20(%rsi),%rcx
        cmp    %rcx, %rdi
        jb     LABEL(fill_192_done)
        vmovdqa %ymm0, 0x20(%rsi)

        lea    0x40(%rsi),%rcx
        cmp    %rcx, %rdi
        jb     LABEL(fill_192_done)
        vmovdqa %ymm0, 0x40(%rsi)

        lea    0x60(%rsi),%rcx
        cmp    %rcx, %rdi
        jb     LABEL(fill_192_done)
        vmovdqa %ymm0, 0x60(%rsi)

LABEL(last_wide_store):
        lea    0x80(%rsi),%rcx
        cmp    %rcx, %rdi
        jb     LABEL(fill_192_done)
        vmovdqa %ymm0, 0x80(%rsi)
LABEL(fill_192_done):
        // Stamp the last word.
        vmovdqu %ymm0,(%rdi)
        vzeroupper
        ret
LABEL(stamp_4):
        vmovdqa %ymm0,(%rsi)
        vmovdqa %ymm0,0x20(%rsi)
        vmovdqa %ymm0,0x40(%rsi)
        vmovdqa %ymm0,0x60(%rsi)
        jmp     LABEL(last_wide_store)
