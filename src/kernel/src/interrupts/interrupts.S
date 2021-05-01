	
	.text
	
	.globl irq00
	.extern irq00Handler
	.globl irq01
	.extern irq01Handler
	.globl irq02
	.extern irq02Handler
	.globl irq03
	.extern irq03Handler
	.globl irq04
	.extern irq04Handler
	.globl irq05
	.extern irq05Handler
	.globl irq06
	.extern irq06Handler
	.globl irq07
	.extern irq07Handler
	.globl irq08
	.extern irq08Handler
	.globl irq09
	.extern irq09Handler
	.globl irq0A
	.extern irq0AHandler
	.globl irq0B
	.extern irq0BHandler
	.globl irq0C
	.extern irq0CHandler
	.globl irq0D
	.extern irq0DHandler
	.globl irq0E
	.extern irq0EHandler
	.globl irq0F
	.extern irq0FHandler
	.globl irq10
	.extern irq10Handler
	.globl irq11
	.extern irq11Handler
	.globl irq12
	.extern irq12Handler
	.globl irq13
	.extern irq13Handler
	.globl irq14
	.extern irq14Handler
	.globl irq15
	.extern irq15Handler
	.globl irq16
	.extern irq16Handler
	.globl irq17
	.extern irq17Handler
	.globl irq18
	.extern irq18Handler
	.globl irq19
	.extern irq19Handler
	.globl irq1A
	.extern irq1AHandler
	.globl irq1B
	.extern irq1BHandler
	.globl irq1C
	.extern irq1CHandler
	.globl irq1D
	.extern irq1DHandler
	.globl irq1E
	.extern irq1EHandler
	.globl irq1F
	
	.extern irqErrorCode

.macro push_macro
	push %r15
	push %r14
	push %r13
	push %r12
	push %r11
	push %r10
	push %r9
	push %r8
	push %rdi
	push %rsi
	push %rsp
	push %rbp
	push %rdx
	push %rcx
	push %rbx
	push %rax
	mov %rsp, %rdi
.endm

.macro pop_macro
	push %rax
	push %rbx
	push %rcx
	push %rdx
	push %rbp
	push %rsp
	push %rsi
	push %rdi
	push %r8
	push %r9
	push %r10
	push %r11
	push %r12
	push %r13
	push %r14
	push %r15
.endm

irq00: /* Division by 0. */
	pushq $0
	push_macro
	call irq00Handler
	pop_macro
	iretq

irq01: /* Debug. */
	pushq $0
	push_macro
	call irq01Handler
	pop_macro
	iretq

irq02: /* NMI */
	pushq $0
	push_macro
	call irq02Handler
	pop_macro
	iretq

irq03: /* Breakpoint */
	pushq $0
	push_macro
	call irq03Handler
	pop_macro
	iretq

irq04: /* Overflow */
	pushq $0
	push_macro
	call irq04Handler
	pop_macro
	iretq

irq05: /* Bound range exceeded */
	pushq $0
	push_macro
	call irq05Handler
	pop_macro
	iretq

irq06: /* Invalid opcde */
	pushq $0
	push_macro
	call irq06Handler
	pop_macro
	iretq

irq07: /* Device not available */
	pushq $0
	push_macro
	call irq07Handler
	pop_macro
	iretq

irq08:  /* Double fault */
	/* Error code: always null. */
	push_macro
	call irq08Handler
	pop_macro
	iretq

irq09: /* Coprocessor segment overrun (deprecated) */
	pushq $0
	push_macro
	call irq09Handler
	pop_macro
	iretq

irq0A: /* Invalid TSS (not relevant) */
	/* Error code. */
	push_macro
	call irq0AHandler
	pop_macro
	iretq

irq0B: /* Segment not present */
	/* Error code. */
	push_macro
	call irq0BHandler
	pop_macro
	iretq

irq0C: /* Stack segment fault*/
	/* Error code. */
	push_macro
	call irq0CHandler
	pop_macro
	iretq

irq0D: /* General protection fault */
	/* Error code. */
	push_macro
	call irq0DHandler
	pop_macro
	iretq

irq0E: /* Page fault */
	/* Error code. */
	push_macro
	call irq0EHandler
	pop_macro
	iretq

irq0F: /* Reserved */
	pushq $0
	push_macro
	call irq0FHandler
	pop_macro
	iretq

irq10: /* x87 floating-point exception (not relevant) */
	pushq $0
	push_macro
	call irq10Handler
	pop_macro
	iretq

irq11: /* Alignment check */
	/* Error code. */
	push_macro
	call irq11Handler
	pop_macro
	iretq

irq12: /* Machine check */
	pushq $0
	push_macro
	call irq12Handler
	pop_macro
	iretq

irq13: /* SIMD floating-point exception */
	pushq $0
	push_macro
	call irq13Handler
	pop_macro
	iretq

irq14: /* Virtualization exception */
	pushq $0
	push_macro
	call irq14Handler
	pop_macro
	iretq

irq15: /* Reserved */
	pushq $0
	push_macro
	call irq15Handler
	pop_macro
	iretq

irq16: /* Reserved */
	pushq $0
	push_macro
	call irq16Handler
	pop_macro
	iretq

irq17: /* Reserved */
	pushq $0
	push_macro
	call irq17Handler
	pop_macro
	iretq

irq18: /* Reserved */
	pushq $0
	push_macro
	call irq18Handler
	pop_macro
	iretq

irq19: /* Reserved */
	pushq $0
	push_macro
	call irq19Handler
	pop_macro
	iretq

irq1A: /* Reserved */
	pushq $0
	push_macro
	call irq1AHandler
	pop_macro
	iretq

irq1B: /* Reserved */
	pushq $0
	push_macro
	call irq1BHandler
	pop_macro
	iretq

irq1C: /* Reserved */
	pushq $0
	push_macro
	call irq1CHandler
	pop_macro
	iretq

irq1D: /* Reserved */
	pushq $0
	push_macro
	call irq1DHandler
	pop_macro
	iretq

irq1E: /* Security exception */
	/* Error code. */
	push_macro
	call irq1EHandler
	pop_macro
	iretq

irq1F: /* Reserved */
	pushq $0
	push_macro
	call irq1FHandler
	pop_macro
	iretq

