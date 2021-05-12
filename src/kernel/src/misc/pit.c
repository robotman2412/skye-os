
#include "pit.h"
#include "kernel.h"

static inline uint8_t inb(uint8_t port) {
	uint8_t ret;
	asm volatile("in %1, %0" : "=a" (ret) : "N" (port));
	return ret;
}

void pitSetup() {
	
}
