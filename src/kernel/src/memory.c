
#include "memory.h"
#include "kernel.h"
#include "heap.h"
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

static struct pmm_resv initialResv;

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
	size_t requiredNumEntries = 4;
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
		warnk("Not enough physical memory!\n");
		warnk("Required ");
		fbPuthex(requiredFreeSize, 16);
		fbNewln();
		kpanic(0);
	}
	// First fill the area with zero.
	for (size_t i = 0; i < (requiredFreeSize + 0xfff) & ~0xff; i++) {
		((uint8_t*) pmmEntryLand)[i] = 0;
	}
	// Now, set up the entire table shits.
	struct pmm_entry *pmmPointer = (struct pmm_entry*) pmmEntryLand;
	struct pmm_entry *pmmPrev = NULL;
	struct pmm_entry *metaPmmEntry = NULL;
	size_t pmmIndex = 0;
	for (size_t i = 0; i < stEntriesLen; i++) {
		struct stivale2_mmap_entry *stEntry = &stEntries[i];
		if (pmmEntryLand == stEntry->base) {
			// The META_ENTRY!!!
			struct pmm_entry entry = {
				.prev = pmmPrev,
				.next = 0,
				.parent = &initialResv,
				.base = stEntry->base,
				.length = (requiredFreeSize + 0xfff) & ~0xfff,
				.type = META,
				.owner = 0
			};
			pmmPointer[pmmIndex] = entry;
			if (pmmPrev) {
				pmmPrev->next = &pmmPointer[pmmIndex];
			}
			pmmPrev = &pmmPointer[pmmIndex];
			metaPmmEntry = &pmmPointer[pmmIndex];
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
				.parent = &initialResv,
				.base = stEntry->base,
				.length = entryLen,
				.type = RESERVED,
				.owner = 0
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
	// Set up reservations.
	initialResv.prev = NULL;
	initialResv.next = NULL;
	initialResv.entry = metaPmmEntry;
	initialResv.capacity = PMM_ENTRIES_PER_BLOCK * metaPmmEntry->length / 0x1000;
	initialResv.numEntries = pmmIndex;
	firstPmmResv = &initialResv;
	lastPmmResv = &initialResv;
}

void printPmm() {
	// Go over the map.
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

// Allocates exactly len memory in 4K increments, starting at address.
// Used to allocate heap space for the kernel.
// Returns null if out of memory to do so.
struct pmm_entry *pmm_allocAt(size_t address, size_t len) {
	if (address & 0xfff) return NULL;
	size_t numBlocks = (len + 0x0fff) / 0x1000;
	size_t foundBlocks = 0;
	struct pmm_entry *bestSpace = NULL; // Best match for amount of space desired.
	struct pmm_entry *next = firstPmmEntry;
	// Iterate over entries.
	while (next) {
		if (next->type == FREE) {
			size_t blocks = next->length / 0x1000;
			if (blocks >= numBlocks && next->base == address) {
				next->type = USED;
				return pmm_split(next, numBlocks);
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
	len *= 0x1000;
	if (block->length <= len) {
		block->type = USED;
		return block;
	} else {
		struct pmm_resv *next = firstPmmResv;
		struct pmm_entry *space = NULL;
		char doAddResv = 0;
		while (next) {
			if (next->numEntries < next->capacity) {
				struct pmm_entry *addr = (struct pmm_entry *) next->entry->base;
				for (size_t i = 0; i < next->capacity; i++) {
					struct pmm_entry *check = &addr[i];
					if (check->type == EMPTY) {
						space = check;
						space->parent = next;
						next->numEntries ++;
						if (next->numEntries >= next->capacity - 1) {
							doAddResv = 1;
						}
						break;
					}
				}
			}
			next = next->next;
		}
		
		if (!space) {
			warnk("No space for PMM entry!\n");
			kpanic(0);
		}
		space->prev = block;
		space->next = block->next;
		space->base = block->base + len;
		space->length = block->length - len;
		space->type = FREE;
		space->owner = 0;
		
		block->length = len;
		space->prev->next = space; // Do not change to block->next = space; that will cause a hang because shit compilers.
		if (space->next) space->next->prev = space;
		else lastPmmEntry = space;
		
		if (doAddResv) {
			struct pmm_resv *resv = kalloc(sizeof(struct pmm_resv));
			resv->prev = lastPmmResv;
			lastPmmResv->next = resv;
			resv->next = 0;
			resv->entry = pmm_allocMost(1); // Allocate just one block.
			resv->entry->type |= PMM_META;
			resv->numEntries = 0;
			resv->capacity = PMM_ENTRIES_PER_BLOCK;
		}
		
		block->type = USED;
		return block;
	}
}

static void pmm_mergeNext(struct pmm_entry *block) {
	while (1) {
		size_t endAddr = block->base + block->length;
		struct pmm_entry *next = block->next;
		if (next && endAddr == next->base) {
			if (next->type == FREE && block->type == FREE || next->owner != 0 && next->owner == block->owner) {
				block->length += next->length;
				block->next = next->next;
				if (block->next) block->next->prev = block;
				next->type = EMPTY;
				next->parent->numEntries --;
			} else {
				return;
			}
		} else {
			return;
		}
	}
}

static void pmm_mergePrev(struct pmm_entry *block) {
	while (1) {
		struct pmm_entry *prev = block->prev;
		if (prev && block->base == prev->base + prev->length) {
			if (prev->type == FREE && block->type == FREE || prev->owner != 0 && prev->owner == block->owner) {
				block->length += prev->length;
				block->base = prev->base;
				block->prev = prev->prev;
				if (block->prev) block->prev->next = block;
				prev->type = EMPTY;
				prev->parent->numEntries --;
			} else {
				return;
			}
		} else {
			return;
		}
	}
}

// Merges the surrounding entries, if applicable.
struct pmm_entry *pmm_merge(struct pmm_entry *block) {
	pmm_mergeNext(block);
	pmm_mergePrev(block);
	return block;
}

// Removes the memory reservation from the entry.
// Merges with neighbouring entries if applicable.
// Returns the new entry that includes the same memory location.
struct pmm_entry *pmm_free(struct pmm_entry *block) {
	block->type = FREE;
	pmm_mergeNext(block);
	pmm_mergePrev(block);
	return block;
}

