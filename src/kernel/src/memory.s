
	
	.globl updateSegments
	
updateSegments:
	push %rax /* Preserve RAX. */
	mov $0x08, %rax
	push %rax /* Set segment. */
	lea .next, %rax
	push %rax /* Set address. */
	lretq
.next:
	mov $0x10, %ax
	mov %ax, %ss
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	pop %rax /* Restore rax. */
	ret
