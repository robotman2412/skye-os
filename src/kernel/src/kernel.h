
/* 
 * Skye OS: kernel.h
 * (c) J. Scheffers, who cares
 * Do whatever you want with it
 */

#ifndef KERNEL_H
#define KERNEL_H
#include <stdint.h>
#include <stddef.h>
#include "stivale2.h"

// Get a stivale2 tag.
void *getTag(struct stivale2_struct *stivale2_struct, uint64_t id);

// Death before the screenbuffer is initialised.
void earlyStageDeath();

// Used by anything to log things.
void logk(char *message);

// Dump all registers and halt.
void kpanic(size_t offendingAddress);

// Try to detect what type of CPU we have.
void detectCPU();

// Kernel entrypoint.
void _start(struct stivale2_struct *stivale2_struct);

#endif //KERNEL_H
