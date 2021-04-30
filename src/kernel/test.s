	.file	"kernel.c"
	.text
	.local	stack
	.comm	stack,4096,32
	.data
	.align 16
	.type	framebuffer_tag, @object
	.size	framebuffer_tag, 22
framebuffer_tag:
	.quad	4525022255252470129
	.quad	0
	.value	0
	.value	0
	.value	32
	.section	.stivale2hdr,"aw"
	.align 32
	.type	stivale_hdr, @object
	.size	stivale_hdr, 32
stivale_hdr:
	.quad	0
	.quad	stack+4096
	.quad	0
	.quad	framebuffer_tag
	.text
	.globl	getTag
	.type	getTag, @function
getTag:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	-24(%rbp), %rax
	movq	128(%rax), %rax
	movq	%rax, -8(%rbp)
	jmp	.L2
.L5:
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	cmpq	%rax, -32(%rbp)
	jne	.L3
	movq	-8(%rbp), %rax
	jmp	.L4
.L3:
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -8(%rbp)
.L2:
	cmpq	$0, -8(%rbp)
	jne	.L5
	movl	$0, %eax
.L4:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	getTag, .-getTag
	.globl	earlyStageDeath
	.type	earlyStageDeath, @function
earlyStageDeath:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
.L7:
/APP
# 50 "kernel.c" 1
	 hlt
# 0 "" 2
/NO_APP
	jmp	.L7
	.cfi_endproc
.LFE1:
	.size	earlyStageDeath, .-earlyStageDeath
	.section	.rodata
.LC0:
	.string	"[   0.0000] "
	.text
	.globl	logk
	.type	logk, @function
logk:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	$-12583105, ttyFgCol(%rip)
	movl	$.LC0, %edi
	call	fbPrint
	movl	$-1, ttyFgCol(%rip)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	fbPrint
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	logk, .-logk
	.section	.rodata
.LC1:
	.string	"Skye OS boot\n"
.LC2:
	.string	"Memory map:\n"
.LC3:
	.string	""
.LC4:
	.string	" -> "
.LC5:
	.string	" USABLE"
.LC6:
	.string	" RESERVED"
.LC7:
	.string	" ACPI RECLAIMABLE"
.LC8:
	.string	" ACPI NVS"
.LC9:
	.string	" BAD MEMORY"
.LC10:
	.string	" BOOTLOADER RECLAIMABLE"
.LC11:
	.string	" KERNEL"
.LC12:
	.string	" FRAMEBUFFER"
.LC13:
	.string	"Usable memory:      "
.LC14:
	.string	"Setting up memory map.\n"
	.align 8
.LC15:
	.string	"If you read this, we didn't crash.\n"
	.text
	.globl	_start
	.type	_start, @function
_start:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movq	%rdi, -72(%rbp)
	movq	-72(%rbp), %rax
	movabsq	$5792862577776396538, %rsi
	movq	%rax, %rdi
	call	getTag
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.L10
	movl	$0, %eax
	call	earlyStageDeath
.L10:
	movq	-24(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, framebuf(%rip)
	movq	-24(%rbp), %rax
	movzwl	24(%rax), %eax
	movzwl	%ax, %eax
	movq	%rax, framebufWidth(%rip)
	movq	-24(%rbp), %rax
	movzwl	26(%rax), %eax
	movzwl	%ax, %eax
	movq	%rax, framebufHeight(%rip)
	movb	$1, doubleTextSize(%rip)
	movq	framebufWidth(%rip), %rcx
	movabsq	$2635249153387078803, %rdx
	movq	%rcx, %rax
	mulq	%rdx
	movq	%rcx, %rax
	subq	%rdx, %rax
	shrq	%rax
	addq	%rdx, %rax
	shrq	$2, %rax
	movq	%rax, %rdx
	movzbl	doubleTextSize(%rip), %eax
	movsbl	%al, %eax
	movl	%eax, %ecx
	shrq	%cl, %rdx
	movq	%rdx, %rax
	movl	%eax, ttyMaxX(%rip)
	movq	framebufHeight(%rip), %rax
	movabsq	$-2049638230412172401, %rdx
	mulq	%rdx
	shrq	$3, %rdx
	movzbl	doubleTextSize(%rip), %eax
	movsbl	%al, %eax
	movl	%eax, %ecx
	shrq	%cl, %rdx
	movq	%rdx, %rax
	movl	%eax, ttyMaxY(%rip)
	movl	$.LC1, %edi
	call	logk
	movl	$.LC2, %edi
	call	logk
	movq	-72(%rbp), %rax
	movabsq	$2416171985333837319, %rsi
	movq	%rax, %rdi
	call	getTag
	movq	%rax, -32(%rbp)
	movq	$0, -16(%rbp)
	jmp	.L11
.L23:
	movq	-32(%rbp), %rcx
	movq	-16(%rbp), %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	leaq	16(%rax), %rcx
	movq	8(%rcx), %rax
	movq	16(%rcx), %rdx
	movq	%rax, -64(%rbp)
	movq	%rdx, -56(%rbp)
	movq	24(%rcx), %rax
	movq	%rax, -48(%rbp)
	movl	$.LC3, %edi
	call	logk
	movq	-64(%rbp), %rax
	movl	$16, %esi
	movq	%rax, %rdi
	call	fbPuthex
	movl	$.LC4, %edi
	call	fbPrint
	movq	-64(%rbp), %rdx
	movq	-56(%rbp), %rax
	addq	%rdx, %rax
	subq	$1, %rax
	movl	$16, %esi
	movq	%rax, %rdi
	call	fbPuthex
	movl	-48(%rbp), %eax
	cmpl	$4098, %eax
	je	.L12
	cmpl	$4098, %eax
	ja	.L13
	cmpl	$4097, %eax
	je	.L14
	cmpl	$4097, %eax
	ja	.L13
	cmpl	$5, %eax
	ja	.L15
	testl	%eax, %eax
	je	.L13
	cmpl	$5, %eax
	ja	.L13
	movl	%eax, %eax
	movq	.L17(,%rax,8), %rax
	jmp	*%rax
	.section	.rodata
	.align 8
	.align 4
.L17:
	.quad	.L13
	.quad	.L21
	.quad	.L20
	.quad	.L19
	.quad	.L18
	.quad	.L16
	.text
.L15:
	cmpl	$4096, %eax
	je	.L22
	jmp	.L13
.L21:
	movl	$.LC5, %edi
	call	fbPrint
	movq	-56(%rbp), %rax
	addq	%rax, -8(%rbp)
	jmp	.L13
.L20:
	movl	$.LC6, %edi
	call	fbPrint
	jmp	.L13
.L19:
	movl	$.LC7, %edi
	call	fbPrint
	jmp	.L13
.L18:
	movl	$.LC8, %edi
	call	fbPrint
	jmp	.L13
.L16:
	movl	$.LC9, %edi
	call	fbPrint
	jmp	.L13
.L22:
	movl	$.LC10, %edi
	call	fbPrint
	jmp	.L13
.L14:
	movl	$.LC11, %edi
	call	fbPrint
	jmp	.L13
.L12:
	movl	$.LC12, %edi
	call	fbPrint
	nop
.L13:
	movl	$0, %eax
	call	fbNewln
	addq	$1, -16(%rbp)
.L11:
	movq	-32(%rbp), %rax
	movq	16(%rax), %rax
	cmpq	%rax, -16(%rbp)
	jb	.L23
	movl	$.LC13, %edi
	call	logk
	movq	-8(%rbp), %rax
	movl	$16, %esi
	movq	%rax, %rdi
	call	fbPuthex
	movl	$0, %eax
	call	fbNewln
	movl	$.LC14, %edi
	call	logk
	movq	-32(%rbp), %rax
	movq	16(%rax), %rax
	movq	-32(%rbp), %rdx
	addq	$24, %rdx
	movq	%rax, %rsi
	movq	%rdx, %rdi
	movl	$0, %eax
	call	setupMemoryMap
	movl	$.LC15, %edi
	call	logk
	movl	$0, %eax
	call	earlyStageDeath
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	_start, .-_start
	.ident	"GCC: (GNU) 9.3.0"
