#if defined(__APPLE__)
.text
.global _libc_memcpy
.p2align  4, 0x90
_libc_memcpy:
        jmp _memcpy

#else

.text
.global libc_memcpy
.p2align  4, 0x90
libc_memcpy:
        jmp memcpy
#endif

#define LABEL(x)     .L##x
#if defined(__APPLE__)
.text
.global _asm_memcpy
.p2align  5, 0x90
_asm_memcpy:
#else
.text
.global asm_memcpy
.p2align  5, 0x90
asm_memcpy:
#endif

// RDI is the dest
// RSI is the src
// RDX is length
  mov  %rdi, %rax
  cmp    $64,%rdx
  ja LABEL(over_64)
  cmp    $16,%rdx
  jae LABEL(16_to_64)

LABEL(below_16):
  cmp    $4,%rdx
  jbe LABEL(0_to_4)
  cmp    $8,%rdx
  jbe LABEL(in_4_to_8)
LABEL(8_to_16):
  movq  (%rsi), %rcx
  movq  %rcx, (%rax)
  movq  -8(%rsi,%rdx), %rcx
  movq  %rcx, -8(%rax,%rdx)
  retq

LABEL(0_to_4):
  // Copy the first two bytes:
  cmp    $0,%rdx
  je      LABEL(exit)
  movb  (%rsi), %cl
  movb  %cl, (%rdi)
  movb  -1(%rsi,%rdx), %cl
  movb  %cl, -1(%rdi,%rdx)
  cmp   $2,%rdx
  jbe   LABEL(exit)
  // Copy the second two bytes, if n > 2.
  movb  1(%rsi), %cl
  movb  %cl, 1(%rdi)
  movb  2(%rsi), %cl
  movb  %cl, 2(%rdi)
  retq
LABEL(in_4_to_8):
  movl  (%rsi), %ecx
  movl  %ecx, (%rdi)
  movl  -4(%rsi,%rdx), %ecx
  movl  %ecx, -4(%rdi,%rdx)
LABEL(exit):
  retq

LABEL(16_to_64):
  cmp    $32, %rdx
  jbe LABEL(16_to_32)

LABEL(32_to_64):
  vmovdqu  (%rsi), %ymm0
  vmovdqu  %ymm0, (%rdi)
  vmovdqu  -32(%rsi,%rdx), %ymm0
  vmovdqu  %ymm0, -32(%rdi,%rdx)
  vzeroupper
  retq

LABEL(16_to_32):
  movups  (%rsi), %xmm0
  movups  %xmm0, (%rdi)
  movups  -16(%rsi,%rdx), %xmm0
  movups  %xmm0, -16(%rdi,%rdx)
  retq

  // Handle buffers over 64 bytes:
LABEL(over_64):
  cmp    $128, %rdx
  ja LABEL(over_128)

  // Copy the last wide word.
  vmovups  -32(%rsi,%rdx), %ymm0

  // Handle cases in the range 64 to 128. This is two unconditional
  // stores (64), 1 conditional store (32), and the one 32 byte store at
  // the end.
  vmovups  (%rsi), %ymm1
  vmovups  32(%rsi), %ymm2

  cmp    $96, %rdx
  jbe    LABEL(64_to_128_done)
  vmovups  64(%rsi), %ymm3
  vmovups  %ymm3, 64(%rax)

.align 4
LABEL(64_to_128_done):
  vmovups  %ymm1, (%rax)
  vmovups  %ymm2, 32(%rax)
  // Store the last wide word.
  vmovups  %ymm0, -32(%rax,%rdx)
  vzeroupper
  retq

LABEL(over_128):
  // Compute the last writeable destination.
  lea -128(%rdx), %rcx
  xor %r8, %r8
.align 16
LABEL(over_128_copy_loop):
  vmovdqu       (%rsi, %r8), %ymm0
  vmovdqu     32(%rsi, %r8), %ymm1
  vmovdqu     64(%rsi, %r8), %ymm2
  vmovdqu     96(%rsi, %r8), %ymm3
  vmovdqu     %ymm0,   (%rdi, %r8)
  vmovdqu     %ymm1, 32(%rdi, %r8)
  vmovdqu     %ymm2, 64(%rdi, %r8)
  vmovdqu     %ymm3, 96(%rdi, %r8)
  add         $128, %r8
  cmp         %rcx, %r8
  jb LABEL(over_128_copy_loop)

// Handle the tail:
  lea    -32(%rdx), %rcx
  cmp    %r8, %rcx
  jb     LABEL(over_128_done)
  vmovdqu     (%rsi, %r8), %ymm0
  vmovdqu     %ymm0,   (%rdi, %r8)
  add         $32, %r8

  cmp         %r8, %rcx
  jb          LABEL(over_128_done)
  vmovdqu     (%rsi, %r8), %ymm0
  vmovdqu     %ymm0,   (%rdi, %r8)
  add         $32, %r8

  cmp         %r8, %rcx
  jb          LABEL(over_128_done)
  vmovdqu     (%rsi, %r8), %ymm0
  vmovdqu     %ymm0,   (%rdi, %r8)

LABEL(over_128_done):
  // Copy the last 32 bytes
  vmovdqu   -32(%rsi, %rdx), %ymm0
  vmovdqu   %ymm0,   -32(%rdi, %rdx)

  vzeroupper
  retq
