
#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include <stdint.h>
#include <stddef.h>

#define IRQ_MODE_INTERRUPT	0x02
#define IRQ_MODE_TRAP		0x03
#define IRQ_MODE_DISABLED	0x00
#define INTERRUPT_TYPE void

struct interrupt_frame {
    uint64_t rax, rbx, rcx, rdx, rbp, rsp0, rsi, rdi;
    uint64_t r8,  r9,  r10, r11, r12, r13, r14, r15;
    uint64_t errorCode;
    uint64_t rip, cs, rflags, rsp, ss;
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

void setTimerInterruptHandler(void(*handler)(void));

extern uint64_t irqReturnAddr;

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

extern void irq00();
extern void irq01();
extern void irq02();
extern void irq03();
extern void irq04();
extern void irq05();
extern void irq06();
extern void irq07();
extern void irq08();
extern void irq09();
extern void irq0A();
extern void irq0B();
extern void irq0C();
extern void irq0D();
extern void irq0E();
extern void irq0F();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
extern void irq16();
extern void irq17();
extern void irq18();
extern void irq19();
extern void irq1A();
extern void irq1B();
extern void irq1C();
extern void irq1D();
extern void irq1E();
extern void irq1F();

#endif //INTERRUPTS_H
