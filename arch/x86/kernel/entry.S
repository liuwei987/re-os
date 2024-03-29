#include "linkage.h"

R15	= 0x00
R14	= 0x08
R13	= 0x10
R12	= 0x18
R11	= 0x20
R10	= 0x28
R9	= 0x30
R8	= 0x38
RBX	= 0x40
RCX	= 0x48
RDX	= 0x50
RSI	= 0x58
RDI	= 0x60
RBP	= 0x68
DS	= 0x70
ES	= 0x78
RAX	= 0x80

FUNC	= 0x88
ERRCODE	= 0x90

RIP	= 0x98
CS	= 0xa0

RFLAGS	= 0xa8
OLDRSP	= 0xb0
OLDSS	= 0xb8

.section .text
RESTORE_ALL:
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rbx
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi
	popq %rbp
	popq %rax
	movq %rax, %ds
	popq %rax
	movq %rax, %es
	popq %rax;
	addq $0x10, %rsp
	iretq

#define GET_CURRENT(reg)	\
	movq $-32768,reg	\
	andq %rsp, reg

ret_from_exception:
	/*GET_CURRENT(%ebx)	need rewrite*/
ENTRY(ret_from_intr)
	jmp RESTORE_ALL	/*need rewrite*/

ENTRY(divide_error)
	pushq $0
	pushq %rax
	leaq do_divide_error(%rip), %rax
	xchgq %rax, (%rsp)

error_code:
	pushq %rax
	movq %es, %rax
	pushq %rax
	movq %ds, %rax
	pushq %rax
	xorq %rax, %rax

	pushq %rbp
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %rbx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	cld
	movq ERRCODE(%rsp), %rsi
	movq FUNC(%rsp), %rdx

	movq $0x10, %rdi
	movq %rdi, %ds
	movq %rdi, %es

	movq %rsp, %rdi
	////GET_CURRENT(%ebx)

	callq *%rdx

	jmp ret_from_exception

ENTRY(nmi)
	pushq %rax
	cld
	pushq %rax
	
	pushq %rax
	movq %es, %rax
	pushq %rax
	movq %ds, %rax
	pushq %rax
	xorq %rax, %rax
	
	pushq %rbp
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %rbx
	pushq %r8;
	pushq %r9;
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	movq $0x10, %rdx
	movq %rdx, %ds
	movq %rdx, %es
	
	movq $0, %rsi
	movq %rsp, %rdi

	callq do_nmi

	jmp RESTORE_ALL

ENTRY(invalid_TSS)
	pushq %rax
	leaq do_invalid_TSS(%rip), %rax
	xchgq %rax, (%rsp)
	jmp error_code

ENTRY(page_fault)
	pushq %rax
	leaq do_page_fault(%rip), %rax
	xchgq %rax, (%rsp)
	jmp error_code
