
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
  cmp    $0x04,%rdx
  jbe LABEL(upto_4)
  cmp    $0x08,%rdx
  jbe LABEL(upto_8)
  cmp    $0x10,%rdx
  jbe LABEL(upto_16)
  cmp    $0x20,%rdx
  jbe LABEL(16_to_32)

// Handle buffers over 32 bytes:
  cmp    $0x80,%rdx
  jae LABEL(over_128)

  xorl  %edi, %edi  // RDI is a running counter.
  lea -0x20(%rdx), %rdx

.align  16
LABEL(over_32_fill):
  vmovups  (%rsi,%rdi), %ymm0
  vmovups  %ymm0, (%rax,%rdi)
  addq  $32, %rcx
  addq  $32, %rdi
  cmpq  %rdx, %rdi
  jb  LABEL(over_32_fill)

  // Copy the last wide word.
  vmovups  (%rsi,%rdx), %ymm0
  vmovups  %ymm0, (%rax,%rdx)
  vzeroupper
  retq

LABEL(upto_4):
  // Copy the first two bytes:
  cmp    $0x00,%rdx
  je      LABEL(exit)
  movb  (%rsi), %cl
  movb  %cl, (%rdi)
  movb  -1(%rsi,%rdx), %cl
  movb  %cl, -1(%rdi,%rdx)
  cmp   $0x02,%rdx
  jbe   LABEL(exit)
  // Copy the second two bytes, if n > 2.
  movb  1(%rsi), %cl
  movb  %cl, 1(%rdi)
  movb  2(%rsi), %cl
  movb  %cl, 2(%rdi)
  retq
LABEL(upto_8):
  movq  (%rsi), %rcx
  movl  (%rsi), %ecx
  movl  %ecx, (%rdi)
  movl  -4(%rsi,%rdx), %ecx
  movl  %ecx, -4(%rdi,%rdx)
  retq
LABEL(upto_16):
  movq  (%rsi), %rcx
  movq  %rcx, (%rax)
  movq  -8(%rsi,%rdx), %rcx
  movq  %rcx, -8(%rax,%rdx)
  retq
LABEL(16_to_32):
  movups  (%rsi), %xmm0
  movups  %xmm0, (%rdi)
  movups  -16(%rsi,%rdx), %xmm0
  movups  %xmm0, -16(%rdi,%rdx)
  retq

LABEL(exit):
  retq

LABEL(over_128):

  // Copy the last 32 bytes
  vmovdqu   -32(%rsi, %rdx), %ymm0
  vmovdqu   %ymm0,   -32(%rdi, %rdx)

  // Compute the last writeable destination.
  lea -0x80(%rdx), %rcx
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
  add         $0x80, %r8
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
  vzeroupper
  retq
