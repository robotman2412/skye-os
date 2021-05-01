
#ifndef MEMORY_H
#define MEMORY_H

#include "stivale2.h"
#include <stdint.h>
#include <stddef.h>

#define PMM_USED	0x01
#define PMM_BAD		0x02
#define PMM_RESVD	0x04
#define PMM_META	0x08

enum memmap_type {
	FREE		= 0x00,
	USED		= 0x01,
	BAD			= 0x03,
	RESERVED	= 0x05,
	META		= 0x09
};

struct pmm_entry {
	struct pmm_entry *prev;
	struct pmm_entry *next;
	size_t base;			// Start address in physical memory, in 4K increments.
	size_t length;			// Length, in 4K increments.
	uint8_t type;			// Type of memmap entry.
} __attribute__((packed));

struct pmm_resv {
	struct pmm_resv *prev;
	struct pmm_resv *next;
	struct pmm_entry *entry; // The PMM entry that reserves this block. This is never in said block.
	size_t numEntries; // The number of entries stored in here. Entries will be relocated so as to have no gaps.
} __attribute__((packed));

#define PMM_ENTRIES_PER_BLOCK 4096 / sizeof(struct pmm_entry)

// PMM entries reserved to... do PMM :P
// Every time there's space for only one more PMM entry in total, we allocate one more of these.
// Eventually, there will also be some defragmenting things related to this.
extern struct pmm_resv *firstPmmResv;
extern struct pmm_resv *lastPmmResv;

// All PMM entries.
extern struct pmm_entry *firstPmmEntry;
extern struct pmm_entry *lastPmmEntry;

// The amount of free blocks left.
extern size_t numBlocksFree;

struct gdt_entry {
	uint16_t limit0;
	uint16_t base0;
	uint8_t base1;
	uint8_t access;
	uint8_t limit1_flags;
	uint8_t base2;
} __attribute__((packed));

// Initial set-up of the memory map, given the stivale2 entries.
void setupMemoryMap(struct stivale2_mmap_entry *stEntries, size_t stEntriesLen);

extern void updateSegments();

// Whether or not this stivale2 entry is usable memory.
char pmm_isStUsable(struct stivale2_mmap_entry *stEntry);

// Whether or not this stivale2 entry is counted in the final memory map.
char pmm_isStCounted(struct stivale2_mmap_entry *stEntry);

// Allocates at most len memory in 4K increments.
struct pmm_entry *pmm_allocMost(size_t len);

// Allocates exactly len memory in 4K increments.
struct pmm_entry *pmm_alloc(size_t len);

// Splits the PMM entry so as to allocate exactly len blocks, if necessary.
struct pmm_entry *pmm_split(struct pmm_entry *block, size_t len);

// Removes the memory reservation from the entry.
struct pmm_entry *pmm_free(struct pmm_entry *block);

#endif //MEMORY_H
