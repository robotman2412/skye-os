
#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "memory.h"

#define MIN_HEAP_SECT_SIZE (sizeof(struct heap_desc) + 32)
#define HEAP_MAGIC_SAUCE 0xcf12fe894fab435
#define HEAP_ALLOCATION_INCREMENT 0x1000
#define HEAP_MEMORY_OWNER 0x8000000000000001

#define HEAP_IS_USED 0x01
#define HEAP_IS_IDENT 0x80

struct heap_desc {
	size_t magic;				// Free checks for this.
	struct heap_desc* prev;
	struct heap_desc* next;
	char flags;					// Obvious.
	size_t length;				// Length excluding descriptor.
} __attribute__((packed));

extern struct heap_desc* firstHeapDesc;
extern struct heap_desc* lastHeapDesc;

// Initialises the kernel heap.
void initKernelHeap();

// Allocate at least size bytes of memory.
void *kalloc(size_t size);

// Free the thingy.
int kfree(void *ptr);

#endif //HEAP_H
