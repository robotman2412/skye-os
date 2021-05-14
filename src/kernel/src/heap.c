
#include "heap.h"
#include "string.h"

struct heap_desc* firstHeapDesc;
struct heap_desc* lastHeapDesc;

// Initialises the kernel heap.
void initKernelHeap() {
	struct pmm_entry *mem = pmm_allocMost(HEAP_ALLOCATION_INCREMENT);
	firstHeapDesc = (struct heap_desc *) mem->base;
	lastHeapDesc = (struct heap_desc *) mem->base;
	mem->owner = HEAP_MEMORY_OWNER;
	firstHeapDesc->magic = HEAP_MAGIC_SAUCE;
	firstHeapDesc->prev = NULL;
	firstHeapDesc->next = NULL;
	firstHeapDesc->length = mem->length - sizeof(struct heap_desc);
}

// Called when the heap is out of memory to support some odd alloc.
static struct heap_desc* heapExpand(struct heap_desc* biggest, size_t minimum) {
	if (minimum < HEAP_ALLOCATION_INCREMENT) {
		minimum = HEAP_ALLOCATION_INCREMENT;
	}
	struct pmm_entry *mem;
	size_t bigAddr = (size_t) biggest + biggest->length + sizeof(struct heap_desc);
	mem = pmm_allocAt(bigAddr, minimum - biggest->length);
	if (!mem) mem = pmm_alloc(minimum);
	else {
		mem->owner = HEAP_MEMORY_OWNER;
		pmm_merge(mem);
		biggest->length += mem->length;
		return biggest;
	}
	mem->owner = HEAP_MEMORY_OWNER;
	pmm_merge(mem);
	struct heap_desc desc = {
		.magic = HEAP_MAGIC_SAUCE,
		.prev = lastHeapDesc,
		.next = NULL,
		.flags = HEAP_IS_IDENT,
		.length = mem->length - sizeof(struct heap_desc)
	};
	lastHeapDesc = (struct heap_desc *) mem->base;
	*lastHeapDesc = desc;
	lastHeapDesc->prev->next = lastHeapDesc;
	return lastHeapDesc;
}

// Allocate at least size bytes of memory.
void *kalloc(size_t size) {
	struct heap_desc *desc = firstHeapDesc;
	struct heap_desc *bestMatch = NULL;
	size_t bestSize = 0;
	struct heap_desc *biggest = NULL;
	size_t biggestSize = 0;
	while (desc) {
		if (!(desc->flags & HEAP_IS_USED)) {
			if (desc->length > biggestSize) {
				biggestSize = desc->length;
				biggest = desc;
			}
			if (desc->length == size) {
				desc->flags |= HEAP_IS_USED;
				return (void *) &desc[1];
			} else if (desc->length >= size && (bestSize == 0 || desc->length < bestSize)) {
				bestSize = desc->length;
				bestMatch = desc;
			}
		}
		desc = desc->next;
	}
	if (!bestMatch) {
		bestMatch = heapExpand(biggest, size + sizeof(struct heap_desc));
	}
	if (!bestMatch) {
		return NULL;
	}
	if (bestMatch->length >= size + MIN_HEAP_SECT_SIZE) {
		struct heap_desc *split = (struct heap_desc *) &((uint8_t *) bestMatch)[size + sizeof(struct heap_desc)];
		split->magic = HEAP_MAGIC_SAUCE;
		split->prev = bestMatch;
		split->next = bestMatch->next;
		split->flags = 0;
		split->length = bestMatch->length - size - sizeof(struct heap_desc);
		split->prev->next = split;
		bestMatch->length = size;
	}
	bestMatch->flags |= HEAP_IS_USED;
	return (void *) &bestMatch[1];
}

void *krealloc(void *ptr, size_t size) {
	if (!ptr) {
		return kalloc(size);
	}
	struct heap_desc *desc = ptr - sizeof(struct heap_desc);
	if (desc->magic != HEAP_MAGIC_SAUCE) {
		void *dest = kalloc(size);
		warnk("Memory not copied: pointer is not on the heap!\n");
		return dest;
	} else if (desc->length >= size) {
		return ptr;
	}
	void *dest = kalloc(size);
	memcpy(dest, ptr, desc->length);
	kfree(ptr);
	return dest;
}

static void checkFree(struct heap_desc *desc) {
	size_t addr = (size_t) desc;
	size_t totalLength = desc->length + sizeof(struct heap_desc);
	size_t nextAddr = addr;
	if (desc == firstHeapDesc || addr & 0xfff) {
		nextAddr = ((addr | 1) + 0xfff) & ~0xfff;
	}
	if (addr + totalLength < nextAddr + 0x1000) return;
	size_t removable = addr + totalLength - nextAddr;
	if ((removable & 0xfff) != 0 && (removable & 0xfff) < MIN_HEAP_SECT_SIZE) {
		removable = (removable & ~0xfff) - 0x1000;
	}
	if (removable < 0x1000) return;
	if (desc == firstHeapDesc || (addr & 0xfff)) {
		desc->length = nextAddr - addr - sizeof(struct heap_desc);
		if (removable & 0xfff) {
			struct heap_desc *next = (struct heap_desc *) (nextAddr + (removable & ~0xfff));
			next->magic = HEAP_MAGIC_SAUCE;
			next->length = (removable & 0xfff) - sizeof(struct heap_desc);
			next->flags = HEAP_IS_IDENT;
			next->prev = desc;
			next->next = desc->next;
			if (next->next) next->next->prev = next;
			else lastHeapDesc = next;
			next->prev->next = next;
		} else {
			
		}
	}
}

// Free the thingy.
int kfree(void *ptr) {
	if (!ptr) {
		return 0;
	}
	struct heap_desc *desc = ptr - sizeof(struct heap_desc);
	if (desc->magic != HEAP_MAGIC_SAUCE || !(desc->flags & HEAP_IS_USED)) {
		return 0;
	}
	desc->flags &= 0xfe;
	if (desc->prev && !(desc->prev->flags & HEAP_IS_USED) && &((uint8_t *)desc->prev)[desc->prev->length + sizeof(struct heap_desc)] == (uint8_t *) desc) {
		desc->prev->length += sizeof(struct heap_desc) + desc->length;
		desc->magic = 0;
		desc->prev->next = desc->next;
		if (desc->next) desc->next->prev = desc->prev;
		else lastHeapDesc = desc->prev;
		desc = desc->prev;
	}
	if (desc->next && !(desc->next->flags & HEAP_IS_USED) && &((uint8_t *)desc)[desc->length + sizeof(struct heap_desc)] == (uint8_t *) desc->next) {
		desc->length += sizeof(struct heap_desc) + desc->next->length;
		desc->next->magic = 0;
		desc->next = desc->next->next;
		if (desc->next) desc->next->prev = desc;
		else lastHeapDesc = desc;
	}
	//checkFree(desc);
	return 1;
}

