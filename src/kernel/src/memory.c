
#include "kernel.h"
#include "memory.h"
#include <stdint.h>
#include <stddef.h>

// 4 Entries of a 64-bit GDT thingy.
static uint64_t gdtMem[3];
static struct gdt_entry gdtConst[] = {
	{.base = 0, .limit = 0x00000000, .type = 0},
	{.base = 0, .limit = 0xffffffff, .type = 0x9A}, // Code.
	{.base = 0, .limit = 0xffffffff, .type = 0x92}, // Data.
	// {.base = 0, .limit = 0xffffffff, .type = 0x89} // TSS is unused.
};

struct lgdt_struct {
	uint16_t size;
	uint64_t offset;
} __attribute__((packed));

static struct lgdt_struct lgdtStruct;

static void updateGDTEntry(uint8_t *target, struct gdt_entry source) {
    // Check the limit to make sure that it can be encoded
    if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF)) {
        logk("An invalid limit was supplied to updateGDTEntry()\n");
		kpanic(0);
    }
    if (source.limit > 65536) {
        // Adjust granularity if required
        source.limit = source.limit >> 12;
        target[6] = 0xC0;
    } else {
        target[6] = 0x40;
    }
 
    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] |= (source.limit >> 16) & 0xF;
 
    // Encode the base 
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;
 
    // And... Type
    target[5] = source.type;
}

void setupMemoryMap(struct stivale2_mmap_entry *stEntries, size_t stEntriesLen) {
	// Time to set up a shitty GDT.
	lgdtStruct.size = sizeof(gdtMem) - 1;
	lgdtStruct.offset = (size_t) &gdtMem;
	// Set gdtMem.
	for (size_t i = 0; i < 3; i++) {
		updateGDTEntry((uint8_t*) &gdtMem[i], gdtConst[i]);
	}
	// Asm time.
	asm volatile (
		" lgdt %0"
		: // No outputs.
		: "m" (lgdtStruct)
	);
	// Now we update segments.
	asm volatile ("movw %cs, 8");
	asm volatile ("movw %ds, 16");
	asm volatile ("movw %ss, 16");
	asm volatile ("movw %es, 16");
	asm volatile ("movw %fs, 16");
	asm volatile ("movw %gs, 16");
	
	// Now, more important things like paging :)
}
