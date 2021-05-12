
/* 
 * Skye OS: kernel.c
 * (c) J. Scheffers, who cares
 * Do whatever you want with it
 */

#include "kernel.h"
#include "stivale2.h"
#include "memory.h"
#include "heap.h"
#include "acpi.h"
#include "timer.h"
#include "framebuffer.h"
#include "interrupts.h"

//#include "testing.h"

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

static char regNames[16][4] = {
	"rax", "rbx", "rcx", "rdx",
	"rsp", "rbp", "rsi", "rdi",
	"r8 ", "r9 ", "r10", "r11",
	"r12", "r13", "r14", "r15"
};

static char segNames[6][6] = {
	" cs: ", " ss: ",
	" ds: ", " es: ",
	" fs: ", " gs: "
};

// Kernel entry point.
void _start(struct stivale2_struct *stivale2_struct) {
	
	// Grab framebuffer info.
	struct stivale2_struct_tag_framebuffer *framebufTag;
	framebufTag = getTag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	if (!framebufTag) {
		// Stop.
		asm ("cli");
		while (1) {
			asm ("hlt");
		};
	}
	
	// Set up the framebuffer.
	framebuf = (uint32_t*) framebufTag->framebuffer_addr;
	framebufWidth = framebufTag->framebuffer_width;
	framebufHeight = framebufTag->framebuffer_height;
	doubleTextSize = 1;
	ttyMaxX = (framebufWidth >> doubleTextSize) / 7;
	ttyMaxY = (framebufHeight >> doubleTextSize) / 9;
	fbFill(ttyBgCol);
	fbDrawIcon();
	
	// Let's do something.
	struct stivale2_struct_tag_memmap *memmapTag;
	memmapTag = getTag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	
	asm ("cli");
	setupMemoryMap(memmapTag->memmap, memmapTag->entries);
	initKernelHeap();
	
	fbSetup();
	logk("Skye OS boot\n");
	logk("Memory map:\n");
	printPmm();
	
	logk("Setting up interrupts.\n");
	setupInterrupts();
	asm ("sti");
	logk("Confusing ACPI.\n");
	
	struct stivale2_struct_tag_rsdp *rsdpTag;
	rsdpTag = getTag(stivale2_struct, STIVALE2_STRUCT_TAG_RSDP_ID);
	if (!rsdpTag) {
		warnk("Missing RDSP!\n");
		kpanic();
	} else {
		confuseAcpi(rsdpTag);
		timerSetup();
	}
	
#ifdef TESTING_H
	doDebugTests();
#endif
	
	// Waste some time.
	for (size_t i = 0; i < 100000000; i++) fbSet(i & 0x3, 0, fbGet(i & 0x3, 0));
	logk("TIME_TEST_LOL\n");
	
	// Fin.
	asm ("cli");
	while (1) {
		asm ("hlt");
	};
}

void kpanic() {
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
	uint64_t temp = __builtin_return_address(0);
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

void kpanici(struct interrupt_frame* data) {
	uint64_t temp = data->rip;
	ttyFgCol = COLOR_RED;
	// RIP
	fbPrint("KERNEL PANIC!\nrip: ");
	fbPuthex(temp, 16);
	// Segment registers
	uint16_t segs[6] = {
		data->cs, data->ss
	};
	asm volatile ("mov %%ds, %0" : "=r" (segs[2]));
	asm volatile ("mov %%es, %0" : "=r" (segs[3]));
	asm volatile ("mov %%fs, %0" : "=r" (segs[4]));
	asm volatile ("mov %%gs, %0" : "=r" (segs[5]));
	// General registers.
	fbNewln();
	for (int i = 0; i < 16; i++) {
		fbNewln();
		fbPrint(regNames[i]);
		fbPrint(": ");
		temp = (&data->rax)[i];
		fbPuthex(temp, 16);
		if (i < 6) {
			fbPrint(segNames[i]);
			fbPuthex(segs[i], 4);
		}
	}
	// Stop.
	asm ("cli");
	while (1) {
		asm ("hlt");
	};
}





