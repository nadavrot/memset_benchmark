
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
	xorl	%edi, %edi          // Zero the counter.
  lea -0x20(%rdx), %rdx

.align	16
LABEL(over_32_fill):
	vmovups	(%rsi,%rdi), %ymm0
	vmovups	%ymm0, (%rax,%rdi)
	addq	$32, %rcx
	addq	$32, %rdi
	cmpq	%rdx, %rdi
	jb	LABEL(over_32_fill)

  // Copy the last wide word.
	vmovups	(%rsi,%rdx), %ymm0
	vmovups	%ymm0, (%rax,%rdx)
	vzeroupper
	retq

LABEL(upto_4):
  // Copy the first two bytes:
  cmp    $0x00,%rdx
  je      LABEL(exit)
	movb	(%rsi), %cl
	movb	%cl, (%rdi)
	movb	-1(%rsi,%rdx), %cl
	movb	%cl, -1(%rdi,%rdx)
  cmp   $0x02,%rdx
  jbe   LABEL(exit)
  // Copy the second two bytes, if n > 2.
  movb	1(%rsi), %cl
	movb	%cl, 1(%rdi)
	movb	2(%rsi), %cl
	movb	%cl, 2(%rdi)
  retq
LABEL(upto_8):
  movq	(%rsi), %rcx
  movl	(%rsi), %ecx
	movl	%ecx, (%rdi)
	movl	-4(%rsi,%rdx), %ecx
	movl	%ecx, -4(%rdi,%rdx)
	retq
LABEL(upto_16):
  movq	(%rsi), %rcx
	movq	%rcx, (%rax)
	movq	-8(%rsi,%rdx), %rcx
	movq	%rcx, -8(%rax,%rdx)
  retq
LABEL(16_to_32):
  movups	(%rsi), %xmm0
	movups	%xmm0, (%rdi)
	movups	-16(%rsi,%rdx), %xmm0
	movups	%xmm0, -16(%rdi,%rdx)
	retq

LABEL(exit):
  retq
