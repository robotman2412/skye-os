
/* 
 * Skye OS: kernel.c
 * (c) J. Scheffers, who cares
 * Do whatever you want with it
 */

#include "stivale2.h"
#include "kernel.h"
#include "graphics/framebuffer.h"
#include "string.h"
#include "memory.h"
#include "interrupts.h"

// Reserve me some stack.
static uint8_t stack[4096];

// Ask for a simple 24bpp framebuffer.
static struct stivale2_header_tag_framebuffer framebuffer_tag = {
	.tag = {
		.identifier	= STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
		.next		= 0
	},
	.framebuffer_width	= 0,
	.framebuffer_height	= 0,
	.framebuffer_bpp	= 32,
};

// Main stivale2 header.
__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
	.entry_point	= 0,
	.stack			= (uintptr_t) stack + sizeof(stack),
	.flags			= 0,
	.tags			= (uintptr_t) &framebuffer_tag
};

// Quick, thingy.
void *getTag(struct stivale2_struct *stivale2_struct, uint64_t id) {
	struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
	while (current_tag) {
		if (current_tag->identifier == id) {
			return current_tag;
		}
		current_tag = (struct stivale2_tag*) current_tag->next;
	}
	return 0;
}

void earlyStageDeath() {
	while (1) {
		asm (" hlt");
	}
}

void logk(char *message) {
	ttyFgCol = COLOR_GREEN;
	fbPrint("[   0.0000] ");
	ttyFgCol = COLOR_WHITE;
	fbPrint(message);
}

static char regNames[16][4] = {
	"rax", "rbx", "rcx", "rdx",
	"rsp", "rbp", "rsi", "rdi",
	"r8 ", "r9 ", "r10", "r11",
	"r12", "r13", "r14", "r15"
};

void kpanic(size_t address) {
	asm volatile ("push %r15");
	asm volatile ("push %r14");
	asm volatile ("push %r13");
	asm volatile ("push %r12");
	asm volatile ("push %r11");
	asm volatile ("push %r10");
	asm volatile ("push %r9");
	asm volatile ("push %r8");
	asm volatile ("push %rdi");
	asm volatile ("push %rsi");
	asm volatile ("push %rsp");
	asm volatile ("push %rbp");
	asm volatile ("push %rdx");
	asm volatile ("push %rcx");
	asm volatile ("push %rbx");
	asm volatile ("push %rax");
	uint64_t temp = address ? address : (uint64_t) __builtin_return_address(0);
	ttyFgCol = COLOR_RED;
	// RIP
	fbPrint("KERNEL PANIC!\nrip: ");
	fbPuthex(temp, 16);
	// General registers.
	fbNewln();
	for (int i = 0; i < 16; i++) {
		fbNewln();
		fbPrint(regNames[i]);
		fbPrint(": ");
		asm volatile ("pop %0" : "=r" (temp));
		fbPuthex(temp, 16);
	}
	// Stop.
	asm ("cli");
	while (1) {
		asm ("hlt");
	};
}

// Kernel entry point.
void _start(struct stivale2_struct *stivale2_struct) {
	
	// Grab framebuffer info.
	struct stivale2_struct_tag_framebuffer *framebufTag;
	framebufTag = getTag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	if (!framebufTag) earlyStageDeath();
	
	// Set up the framebuffer.
	framebuf = (uint32_t*) framebufTag->framebuffer_addr;
	framebufWidth = framebufTag->framebuffer_width;
	framebufHeight = framebufTag->framebuffer_height;
	doubleTextSize = 1;
	ttyMaxX = (framebufWidth / 7) >> doubleTextSize;
	ttyMaxY = (framebufHeight / 9) >> doubleTextSize;
	
	// Let's do something.
	logk("Skye OS boot\n");
	logk("Memory map:\n");
	
	struct stivale2_struct_tag_memmap *memmapTag;
	memmapTag = getTag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	
	size_t usableMemory = 0;
	for (size_t i = 0; i < memmapTag->entries; i++) {
		struct stivale2_mmap_entry mapEntry = memmapTag->memmap[i];
		logk("");
		fbPuthex(mapEntry.base, 16);
		fbPrint(" -> ");
		fbPuthex(mapEntry.base + mapEntry.length - 1, 16);
		switch (mapEntry.type) {
			case (STIVALE2_MMAP_USABLE):
				fbPrint(" USABLE");
				usableMemory += mapEntry.length;
				break;
			case (STIVALE2_MMAP_RESERVED):
				fbPrint(" RESERVED");
				break;
			case (STIVALE2_MMAP_ACPI_RECLAIMABLE):
				fbPrint(" ACPI RECLAIMABLE");
				break;
			case (STIVALE2_MMAP_ACPI_NVS):
				fbPrint(" ACPI NVS");
				break;
			case (STIVALE2_MMAP_BAD_MEMORY):
				fbPrint(" BAD MEMORY");
				break;
			case (STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE):
				fbPrint(" BOOTLOADER RECLAIMABLE");
				break;
			case (STIVALE2_MMAP_KERNEL_AND_MODULES):
				fbPrint(" KERNEL");
				break;
			case (STIVALE2_MMAP_FRAMEBUFFER):
				fbPrint(" FRAMEBUFFER");
				break;
		}
		fbNewln();
	}
	
	logk("Usable memory:      ");
	fbPuthex(usableMemory, 16);
	fbNewln();
	logk("Setting up memory map.\n");
	asm ("cli");
	setupMemoryMap(memmapTag->memmap, memmapTag->entries);
	logk("Setting up interrupts.\n");
	setupInterrupts();
	asm ("sti");
	logk("TEST0\n");
	//int ax = 123456;
	//int sm = 2;
	//fbPuthex(ax / sm, 16);
	//fbNewln();
	asm volatile ("int $0x00");
	logk("TEST1\n");
	
	// Fin.
	earlyStageDeath();
	
}





