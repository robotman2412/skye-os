
#ifndef PORTS_H
#define PORTS_H

#include <stdint.h>

static inline uint8_t inb(uint16_t port) {
	uint8_t data;
	asm volatile("inb %1, %0" : "=a"(data) : "Nd"(port) : "memory");
	return data;
}

static inline void outb(uint16_t port, uint8_t data) {
	asm volatile("outb %0, %1" :: "a"(data), "Nd"(port) : "memory");
}

static inline uint8_t inw(uint16_t port) {
	uint16_t data;
	asm volatile("inw %1, %0" : "=a"(data) : "d"(port) : "memory");
	return data;
}

static inline void outw(uint16_t port, uint16_t data) {
	asm volatile("outw %0, %1" :: "a"(data), "d"(port) : "memory");
}

#endif // PORTS_H
