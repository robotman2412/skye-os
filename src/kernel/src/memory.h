
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

#define MMAP_USED	0x01
#define MMAP_BAD	0x02
#define MMAP_RESVD	0x04

enum memmap_type {
	FREE		= 0x00,
	USED		= 0x01,
	BAD			= 0x03,
	RESERVED	= 0x05
};

struct memmap_entry {
	struct memmap_entry *prev;
	struct memmap_entry *next;
	size_t base;		// Start address in physical memory.
	size_t length;		// Length.
	size_t mappedTo;	// Start address in virtual memory, if any.
	uint8_t type;		// Type of memmap entry.
	void *owner;		// Owner of this memmap entry, points to task_struct if nonnull, owned by kernel otherwise.
} __attribute__((packed));

extern struct memmap_entry *firstMemmapEntry;
extern struct memmap_entry *lastMemmapEntry;

struct gdt_entry {
	uint32_t base;
	uint32_t limit;
	uint8_t type;
} __attribute__((packed));

// Initial set-up of the memory map, given the stivale2 entries.
void setupMemoryMap(struct stivale2_mmap_entry *stEntries, size_t stEntriesLen);

#endif //MEMORY_H
