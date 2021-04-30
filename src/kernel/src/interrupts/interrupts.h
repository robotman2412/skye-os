
#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include <stdint.h>
#include <stddef.h>

#define IRQ_MODE_INTERRUPT	0x02
#define IRQ_MODE_TRAP		0x03
#define IRQ_MODE_DISABLED	0x00
#define INTERRUPT_TYPE __attribute__((interrupt)) void

struct interrupt_frame {
  uint64_t ip;
  uint64_t cs;
  uint64_t flags;
  uint64_t sp;
  uint64_t ss;
} __attribute__((packed));

struct idt_entry {
	uint16_t offset0;
	uint16_t selector;
	uint8_t unused0;
	uint8_t attributes;
	uint16_t offset1;
	uint32_t offset2;
	uint32_t unused1;
} __attribute__((packed));

void setupInterrupts();

void setInterrupt(struct idt_entry *idt, void(*handler)(struct interrupt_frame*), char mode);

extern uint64_t irqReturnAddr;
extern uint64_t irqErrorCode;
extern uint64_t regtmp0;
extern uint64_t regtmp1;
extern uint64_t regtmp2;
extern uint64_t regtmp3;
extern uint64_t regtmp4;
extern uint64_t regtmp5;
extern uint64_t regtmp6;
extern uint64_t regtmp7;
extern uint64_t regtmp8;
extern uint64_t regtmp9;
extern uint64_t regtmp10;
extern uint64_t regtmp11;
extern uint64_t regtmp12;
extern uint64_t regtmp13;
extern uint64_t regtmp14;
extern uint64_t regtmp15;

extern void(*interruptTable[32])(void);

INTERRUPT_TYPE irq00Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq01Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq02Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq03Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq04Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq05Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq06Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq07Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq08Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq09Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq0AHandler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq0BHandler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq0CHandler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq0DHandler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq0EHandler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq0FHandler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq10Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq11Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq12Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq13Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq14Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq15Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq16Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq17Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq18Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq19Handler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq1AHandler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq1BHandler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq1CHandler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq1DHandler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq1EHandler(struct interrupt_frame* frame);
INTERRUPT_TYPE irq1FHandler(struct interrupt_frame* frame);

#endif //INTERRUPTS_H
