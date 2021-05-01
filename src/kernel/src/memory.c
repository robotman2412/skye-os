
#include "memory.h"
#include "kernel.h"
#include "framebuffer.h"
#include <stdint.h>
#include <stddef.h>

// 4 Entries of a 64-bit GDT thingy.
static struct gdt_entry gdtMem[] = {
	{
		.base0 = 0,
		.limit0 = 0,
		.base1 = 0,
		.access = 0,
		.limit1_flags = 0x00,
		.base2 = 0
	},
	{ // Code.
		.base0 = 0,
		.limit0 = 0,
		.base1 = 0,
		.access = 0x9A,
		.limit1_flags = 0x20,
		.base2 = 0
	},
	{ // Data.
		.base0 = 0,
		.limit0 = 0,
		.base1 = 0,
		.access = 0x92,
		.limit1_flags = 0x00,
		.base2 = 0
	},
	{ // TSS
		.base0 = 0,
		.limit0 = 0,
		.base1 = 0,
		.access = 0,
		.limit1_flags = 0x00,
		.base2 = 0
	}
};

struct gdtr_struct {
	uint16_t size;
	uint64_t offset;
} __attribute__((packed));

static struct gdtr_struct lgdtStruct;

struct pmm_resv *firstPmmResv;
struct pmm_resv *lastPmmResv;

struct pmm_entry *firstPmmEntry;
struct pmm_entry *lastPmmEntry;

size_t numBlocksFree;

void setupMemoryMap(struct stivale2_mmap_entry *stEntries, size_t stEntriesLen) {
	// Time to set up a shitty GDT.
	lgdtStruct.size = sizeof(gdtMem) - 1;
	lgdtStruct.offset = (size_t) &gdtMem;
	// Asm time.
	asm volatile (
		" lgdt %0"
		: // No outputs.
		: "m" (lgdtStruct)
	);
	// Now we update segments.
	updateSegments();
	
	// Now, time to make up our memory map.
	// First, find how many entries we need to be able to store.
	size_t requiredNumEntries = 2;
	for (size_t i = 0; i < stEntriesLen; i++) {
		requiredNumEntries += pmm_isStCounted(&stEntries[i]);
	}
	size_t requiredFreeSize = requiredNumEntries * sizeof(struct pmm_entry);
	// Now, find the first usable memory big enough for this.
	size_t pmmEntryLand = 0;
	for (size_t i = 0; i < stEntriesLen; i++) {
		struct stivale2_mmap_entry *stEntry = &stEntries[i];
		if (pmm_isStUsable(stEntry) && stEntry->length >= requiredFreeSize) {
			pmmEntryLand = stEntry->base;
			break;
		}
	}
	// Ensure we actually have that.
	if (pmmEntryLand == 0) {
		logk("Not enough physical memory!\n");
		logk("Required ");
		fbPuthex(requiredFreeSize, 16);
		fbNewln();
		kpanic(0);
	}
	// Now, set up the entire table shits.
	struct pmm_entry *pmmPointer = (struct pmm_entry*) pmmEntryLand;
	struct pmm_entry *pmmPrev = 0;
	size_t pmmIndex = 0;
	for (size_t i = 0; i < stEntriesLen; i++) {
		struct stivale2_mmap_entry *stEntry = &stEntries[i];
		if (pmmEntryLand == stEntry->base) {
			// The META_ENTRY!!!
			struct pmm_entry entry = {
				.prev = pmmPrev,
				.next = 0,
				.base = stEntry->base,
				.length = (requiredFreeSize + 0xfff) & ~0xfff,
				.type = META
			};
			pmmPointer[pmmIndex] = entry;
			if (pmmPrev) {
				pmmPrev->next = &pmmPointer[pmmIndex];
			}
			pmmPrev = &pmmPointer[pmmIndex];
			pmmIndex ++;
			if (entry.length < stEntry->length) {
				entry.prev = pmmPrev;
				entry.next = 0;
				entry.base = stEntry->base + entry.length;
				entry.length = stEntry->length - entry.length;
				entry.type = FREE;
				pmmPointer[pmmIndex] = entry;
				pmmPrev->next = &pmmPointer[pmmIndex];
				pmmPrev = &pmmPointer[pmmIndex];
				pmmIndex ++;
			}
		} else if (pmm_isStCounted(stEntry)) {
			// Other entries.
			size_t entryLen = stEntry->length;
			if (stEntry->base & 0xfff) {
				entryLen = (entryLen + stEntry->base + 0xfff) & ~0xfff - stEntry->base;
			} else if (entryLen & 0xfff) {
				entryLen = (entryLen + 0xfff) & ~0xfff;
			}
			struct pmm_entry entry = {
				.prev = pmmPrev,
				.next = 0,
				.base = stEntry->base,
				.length = entryLen,
				.type = RESERVED
			};
			switch (stEntry->type) {
				case (STIVALE2_MMAP_USABLE):
					entry.type = FREE;
					break;
				case (STIVALE2_MMAP_RESERVED):
					entry.type = RESERVED;
					break;
				case (STIVALE2_MMAP_BAD_MEMORY):
					entry.type = BAD;
					break;
				case (STIVALE2_MMAP_KERNEL_AND_MODULES):
					entry.type = USED;
					break;
			}
			pmmPointer[pmmIndex] = entry;
			if (pmmPrev) {
				pmmPrev->next = &pmmPointer[pmmIndex];
			}
			pmmPrev = &pmmPointer[pmmIndex];
			pmmIndex ++;
		}
	}
	firstPmmEntry = pmmPointer;
	lastPmmEntry = pmmPrev;
	// Go over the map again.
	struct pmm_entry *next = firstPmmEntry;
	while (next) {
		logk("");
		fbPuthex(next->base, 16);
		fbPrint(" -> ");
		fbPuthex(next->base + next->length - 1, 16);
		fbPutc(' ');
		fbPutc(next->type & PMM_USED  ? 'U' : '-');
		fbPutc(next->type & PMM_BAD   ? 'B' : '-');
		fbPutc(next->type & PMM_RESVD ? 'R' : '-');
		fbPutc(next->type & PMM_META  ? 'M' : '-');
		fbNewln();
		next = next->next;
	}
}

// Whether or not this stivale2 entry is usable memory.
char pmm_isStUsable(struct stivale2_mmap_entry *stEntry) {
	switch (stEntry->type) {
		case (STIVALE2_MMAP_USABLE):
			return 1;
			break;
		case (STIVALE2_MMAP_RESERVED):
			return 0;
			break;
		case (STIVALE2_MMAP_ACPI_RECLAIMABLE):
			return 0;
			break;
		case (STIVALE2_MMAP_ACPI_NVS):
			return 0;
			break;
		case (STIVALE2_MMAP_BAD_MEMORY):
			return 0;
			break;
		case (STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE):
			return 0;
			break;
		case (STIVALE2_MMAP_KERNEL_AND_MODULES):
			return 0;
			break;
		case (STIVALE2_MMAP_FRAMEBUFFER):
			return 0;
			break;
	}
	return 0;
}

// Whether or not this stivale2 entry is counted in the final memory map.
char pmm_isStCounted(struct stivale2_mmap_entry *stEntry) {
	switch (stEntry->type) {
		case (STIVALE2_MMAP_USABLE):
			return 1;
			break;
		case (STIVALE2_MMAP_RESERVED):
			return 1;
			break;
		case (STIVALE2_MMAP_ACPI_RECLAIMABLE):
			return 0;
			break;
		case (STIVALE2_MMAP_ACPI_NVS):
			return 0;
			break;
		case (STIVALE2_MMAP_BAD_MEMORY):
			return 1;
			break;
		case (STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE):
			return 0;
			break;
		case (STIVALE2_MMAP_KERNEL_AND_MODULES):
			return 1;
			break;
		case (STIVALE2_MMAP_FRAMEBUFFER):
			return 0;
			break;
	}
	return 0;
}

// Allocates at most len physical memory, in 4K block increments.
// Used to allocate virtual memory.
// Returns null if out of memory to do so.
struct pmm_entry *pmm_allocMost(size_t len) {
	size_t numBlocks = (len + 0x0fff) / 0x1000;
	size_t foundBlocks = 0;
	struct pmm_entry *bestSpace = NULL; // Best match for amount of space desired.
	struct pmm_entry *next = firstPmmEntry;
	// Iterate over entries.
	while (next) {
		if (next->type == FREE) {
			size_t blocks = next->length / 0x1000;
			if (blocks == numBlocks) {
				next->type = USED;
				return next;
			} if (foundBlocks < numBlocks && blocks > foundBlocks) {
				foundBlocks = blocks;
				bestSpace = next;
			} else if (foundBlocks > numBlocks && blocks >= numBlocks && blocks < foundBlocks) {
				foundBlocks = blocks;
				bestSpace = next;
			}
		}
		next = next->next;
	}
	// Did we find anything?
	if (!bestSpace) {
		return NULL;
	}
	// Now, split the block and return it.
	return pmm_split(bestSpace, numBlocks);
}

// Allocates exactly len physical memory, in 4K block increments.
// Used to allocate heap space for the kernel.
// Returns null if out of memory to do so.
struct pmm_entry *pmm_alloc(size_t len) {
	size_t numBlocks = (len + 0x0fff) / 0x1000;
	size_t foundBlocks = 0;
	struct pmm_entry *bestSpace = NULL; // Best match for amount of space desired.
	struct pmm_entry *next = firstPmmEntry;
	// Iterate over entries.
	while (next) {
		if (next->type == FREE) {
			size_t blocks = next->length / 0x1000;
			if (blocks == numBlocks) {
				next->type = USED;
				return next;
			} if (foundBlocks == 0 && blocks >= numBlocks) {
				foundBlocks = blocks;
				bestSpace = next;
			} else if (foundBlocks > numBlocks && blocks >= numBlocks && blocks < foundBlocks) {
				foundBlocks = blocks;
				bestSpace = next;
			}
		}
		next = next->next;
	}
	// Did we find anything?
	if (!bestSpace) {
		return NULL;
	}
	// Now, split the block and return it.
	return pmm_split(bestSpace, numBlocks);
}

// Splits the PMM entry so as to allocate exactly len blocks, if necessary.
struct pmm_entry *pmm_split(struct pmm_entry *block, size_t len) {
	if (block->length <= len) {
		block->type = USED;
		return block;
	} else {
		// TODO.
		block->type = USED;
		return block;
	}
}

// Removes the memory reservation from the entry.
// Merges with neighbouring entries if applicable.
// Returns the new entry that includes the same memory location.
struct pmm_entry *pmm_free(struct pmm_entry *block) {
	block->type = FREE;
	return block;
}

