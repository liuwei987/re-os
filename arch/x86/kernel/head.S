
.section .text

.globl _start
_start:
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %fs
	mov $0x7e00, %esp

	lgdt GDT_POINTER(%rip)
	lidt IDT_POINTER(%rip)

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %fs
	mov %ax, %gs
	mov $0x7e00, %rsp

	// load cr3
	movq $0x101000, %rax
	movq %rax, %cr3
	movq switch_seg(%rip), %rax
	pushq $0x08
	pushq %rax
	lretq

switch_seg:
	.quad entry64

entry64:
	movq $0x10, %rax
	movq %rax, %ss
	movq %rax, %ds
	movq %rax, %es
	//movq %rax, %fs
	movq %rax, %gs
	movq $0xffff800000007e00, %rsp

# construct IDT descriptor and fill up the IDT
Setup_IDT:
	leaq ignore_intr(%rip), %rdx
	movq $(0x08 << 16), %rax    # segment selector(index 1 of Kernel Code 64bit Segment 08 in GDT)
	movw %dx, %ax
	movq $(0x8e00 << 32), %rcx  # 0x8e00(interrupt gate in IDT), 0x8f00(trap gate in IDT)
	addq %rcx, %rax
	movl %edx, %ecx
	shrl $16, %ecx
	shlq $48, %rcx
	addq %rcx, %rax
	shrq $32, %rdx
	leaq IDT_Table(%rip), %rdi
	mov $256, %rcx
rep_fill_IDT:
	movq %rax, (%rdi)
	movq %rdx, 8(%rdi)          # 16 bytes IDT descriptor
	addq $0x10, %rdi
	dec %rcx
	jne rep_fill_IDT

setup_TSS64:
	leaq	TSS64_Table(%rip),	%rdx
	xorq	%rax,	%rax
	xorq	%rcx,	%rcx
	movq	$0x89,	%rax
	shlq	$40,	%rax
	movl	%edx,	%ecx
	shrl	$24,	%ecx
	shlq	$56,	%rcx
	addq	%rcx,	%rax
	xorq	%rcx,	%rcx
	movl	%edx,	%ecx
	andl	$0xffffff,	%ecx
	shlq	$16,	%rcx
	addq	%rcx,	%rax
	addq	$103,	%rax
	leaq	GDT_Table(%rip),	%rdi
	movq	%rax,	64(%rdi)
	shrq	$32,	%rdx
	movq	%rdx,	72(%rdi)

	//mov	$0x40,	%ax
	//ltr	%ax

	movq go_to_kernel(%rip), %rax /* movq address */
	pushq $0x08
	pushq %rax
	lretq

go_to_kernel:
	.quad start_kernel

ignore_intr:
	cld
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %rbp
	pushq %rdi
	pushq %rsi

	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	mov %es, %rax
	pushq %rax
	mov %ds, %rax
	pushq %rax

	mov $0x10, %rax
	mov %rax, %es
	mov %rax, %ds

	leaq intr_msg(%rip), %rdx
	#leaq intr_msg(%rip), %rax
	#pushq %rax
	#movq %rax, %rdx
	movq $0x00000000, %rsi
	movq $0xffffff00, %rdi
	movq $0, %rax
	call color_printk
	#addq $0x8, %rsp

Loop:
	jmp Loop

	popq %rax
	movq %rax, %ds
	popq %rax
	movq %rax, %es

	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rsi
	popq %rdi
	popq %rbp
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq

intr_msg:
	.asciz "Unknow interrrupt or fault at RIP\n"
// init page

.align 8
.org 0x1000

__PML4E:
	.quad 0x102007
	.fill 255,8,0
	.quad 0x102007
	.fill 255,8,0

.org 0x2000
__PDPTE:
	.quad 0x103003
	.fill 511,8,0

.org 0x3000
__PDE:
	.quad 0x000083
	.quad 0x200083
	.quad 0x400083
	.quad 0x600083
	.quad 0x800083
	.quad 0xe0000083
	.quad 0xe0200083
	.quad 0xe0400083
	.quad 0xe0600083
	.quad 0xe0800083
	.quad 0xe0a00083
	.quad 0xe0c00083
	.quad 0xe0e00083
	.fill 499,8,0

.section .data

// init segment

.globl GDT_Table
GDT_Table:
	.quad 0x0000000000000000  /* 0 NULL descriptor */
	.quad 0x0020980000000000  /* 1 Kernel Code 64bit Segment 08   */
	.quad 0x0000920000000000  /* 2 Kernel Data 64bit Segment 0x10 */
	.quad 0x0020f80000000000  /* 3 User Code 64bit Segment 0x18   */
	.quad 0x0000f20000000000  /* 4 User Data 64bit Segment 0x20   */
	.quad 0x00cf9a000000ffff  /* 5 Kernel Code 32bit segment 0x28 */
	.quad 0x00cf92000000ffff  /* 6 Kernel Data 32bit segment 0x30 */
	.fill 10,8,0              /* 8~9 Tss(jmp on segment <7>) in long mode 128bit 40 */
GDT_END:

GDT_POINTER:
GDT_LIMIT: .word GDT_END - GDT_Table - 1
GDT_BASE:  .quad GDT_Table

.globl IDT_Table
IDT_Table:
	.fill 512,8,0
IDT_END:

IDT_POINTER:
IDT_LIMIT: .word IDT_END - IDT_Table - 1
IDT_BASE:  .quad IDT_Table

.globl TSS64_Table
TSS64_Table:
	.fill 13,8,0
TSS64_END:

TSS64_POINTER:
TSS64_LIMIT: .word TSS64_END - TSS64_Table - 1
TSS64_BASE:  .quad TSS64_Table
