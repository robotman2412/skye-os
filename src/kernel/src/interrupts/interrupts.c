
#include "interrupts.h"
#include "kernel.h"
#include <stdint.h>
#include <stddef.h>

uint64_t regtmp0;
uint64_t regtmp1;
uint64_t regtmp2;
uint64_t regtmp3;
uint64_t regtmp4;
uint64_t regtmp5;
uint64_t regtmp6;
uint64_t regtmp7;
uint64_t regtmp8;
uint64_t regtmp9;
uint64_t regtmp10;
uint64_t regtmp11;
uint64_t regtmp12;
uint64_t regtmp13;
uint64_t regtmp14;
uint64_t regtmp15;
uint64_t irqReturnAddr;
uint64_t irqErrorCode;

static struct idt_entry idtTable[256];

static struct lidt_struct {
	uint16_t limit;
	uint64_t base;
} lidt;

void setupInterrupts() {
	// Set interrupt defaults.
	for (int i = 0; i < 256; i++) {
		setInterrupt(&idtTable[i], NULL, IRQ_MODE_DISABLED);
	}
	
	// Division by zero.
	setInterrupt(&idtTable[0], &irq00Handler, IRQ_MODE_INTERRUPT);
	// Debug.
	setInterrupt(&idtTable[0], &irq01Handler, IRQ_MODE_INTERRUPT);
	// NMI.
	setInterrupt(&idtTable[0], &irq02Handler, IRQ_MODE_INTERRUPT);
	// Breakpoint.
	setInterrupt(&idtTable[0], &irq03Handler, IRQ_MODE_INTERRUPT);
	// Overflow.
	setInterrupt(&idtTable[0], &irq04Handler, IRQ_MODE_INTERRUPT);
	// Bound range exceeded.
	setInterrupt(&idtTable[0], &irq05Handler, IRQ_MODE_INTERRUPT);
	// Invalid opcode.
	setInterrupt(&idtTable[0], &irq06Handler, IRQ_MODE_INTERRUPT);
	// Device not available.
	setInterrupt(&idtTable[0], &irq07Handler, IRQ_MODE_INTERRUPT);
	// Double fault.
	setInterrupt(&idtTable[0], &irq08Handler, IRQ_MODE_INTERRUPT);
	// Coprocessor segment overrun.
	//setInterrupt(&idtTable[0], &irq09Handler, IRQ_MODE_INTERRUPT);
	// Invalid TSS
	//setInterrupt(&idtTable[0], &irq0AHandler, IRQ_MODE_INTERRUPT);
	// Segment not present.
	setInterrupt(&idtTable[0], &irq0BHandler, IRQ_MODE_INTERRUPT);
	// Stack-segment fault.
	setInterrupt(&idtTable[0], &irq0CHandler, IRQ_MODE_INTERRUPT);
	// General protection fault.
	setInterrupt(&idtTable[0], &irq0DHandler, IRQ_MODE_INTERRUPT);
	// Page fault.
	setInterrupt(&idtTable[0], &irq0EHandler, IRQ_MODE_INTERRUPT);
	// Reserved.
	//setInterrupt(&idtTable[0], &irq0FHandler, IRQ_MODE_INTERRUPT);
	
	asm volatile ("sidt %0" : "=m" (lidt));
	
	// Set interrupt descriptor table.
	lidt.limit = sizeof(idtTable) - 1;
	lidt.base = (size_t) &idtTable;
	
	asm volatile ("lidt %0" :: "m" (lidt));
}

void setInterrupt(struct idt_entry *idt, void(*handler)(struct interrupt_frame*), char mode) {
	size_t offset = (size_t) handler;
	idt->offset0 = (uint16_t) (offset & 0xffff);
	idt->selector = 8;
	idt->unused0 = 0;
	idt->attributes = 0x0e | (mode & 1) | ((mode << 6) & 0x80);
	idt->offset1 = (uint16_t) ((offset >> 16) & 0xffff);
	idt->offset2 = (uint32_t) ((offset >> 32) & 0xffffffff);
	idt->unused1 = 0;
}

void checkErrk() {
	kpanic(irqReturnAddr);
}

INTERRUPT_TYPE irq00Handler(struct interrupt_frame* frame) {
	logk("Division by zero.\n");
	checkErrk();
}

INTERRUPT_TYPE irq01Handler(struct interrupt_frame* frame) {
	logk("irq01\n");
}

INTERRUPT_TYPE irq02Handler(struct interrupt_frame* frame) {
	logk("irq02\n");
}

INTERRUPT_TYPE irq03Handler(struct interrupt_frame* frame) {
	logk("irq03\n");
}

INTERRUPT_TYPE irq04Handler(struct interrupt_frame* frame) {
	logk("irq04\n");
}

INTERRUPT_TYPE irq05Handler(struct interrupt_frame* frame) {
	logk("irq05\n");
}

INTERRUPT_TYPE irq06Handler(struct interrupt_frame* frame) {
	logk("irq06\n");
}

INTERRUPT_TYPE irq07Handler(struct interrupt_frame* frame) {
	logk("irq07\n");
}

INTERRUPT_TYPE irq08Handler(struct interrupt_frame* frame) {
	logk("irq08\n");
}

INTERRUPT_TYPE irq09Handler(struct interrupt_frame* frame) {
	logk("irq09\n");
}

INTERRUPT_TYPE irq0AHandler(struct interrupt_frame* frame) {
	logk("irq0A\n");
}

INTERRUPT_TYPE irq0BHandler(struct interrupt_frame* frame) {
	logk("irq0B\n");
}

INTERRUPT_TYPE irq0CHandler(struct interrupt_frame* frame) {
	logk("irq0C\n");
}

INTERRUPT_TYPE irq0DHandler(struct interrupt_frame* frame) {
	logk("irq0D\n");
}

INTERRUPT_TYPE irq0EHandler(struct interrupt_frame* frame) {
	logk("irq0E\n");
}

INTERRUPT_TYPE irq0FHandler(struct interrupt_frame* frame) {
	logk("irq0F\n");
}

INTERRUPT_TYPE irq10Handler(struct interrupt_frame* frame) {
	logk("irq10\n");
}

INTERRUPT_TYPE irq11Handler(struct interrupt_frame* frame) {
	logk("irq11\n");
}

INTERRUPT_TYPE irq12Handler(struct interrupt_frame* frame) {
	logk("irq12\n");
}

INTERRUPT_TYPE irq13Handler(struct interrupt_frame* frame) {
	logk("irq13\n");
}

INTERRUPT_TYPE irq14Handler(struct interrupt_frame* frame) {
	logk("irq14\n");
}

INTERRUPT_TYPE irq15Handler(struct interrupt_frame* frame) {
	logk("irq15\n");
}

INTERRUPT_TYPE irq16Handler(struct interrupt_frame* frame) {
	logk("irq16\n");
}

INTERRUPT_TYPE irq17Handler(struct interrupt_frame* frame) {
	logk("irq17\n");
}

INTERRUPT_TYPE irq18Handler(struct interrupt_frame* frame) {
	logk("irq18\n");
}

INTERRUPT_TYPE irq19Handler(struct interrupt_frame* frame) {
	logk("irq19\n");
}

INTERRUPT_TYPE irq1AHandler(struct interrupt_frame* frame) {
	logk("irq1A\n");
}

INTERRUPT_TYPE irq1BHandler(struct interrupt_frame* frame) {
	logk("irq1B\n");
}

INTERRUPT_TYPE irq1CHandler(struct interrupt_frame* frame) {
	logk("irq1C\n");
}

INTERRUPT_TYPE irq1DHandler(struct interrupt_frame* frame) {
	logk("irq1D\n");
}

INTERRUPT_TYPE irq1EHandler(struct interrupt_frame* frame) {
	logk("irq1E\n");
}

INTERRUPT_TYPE irq1FHandler(struct interrupt_frame* frame) {
	logk("irq1F\n");
}

