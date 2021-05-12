
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "hpet.h"
#include "pit.h"

void timerSetup();
void resetWatchdog();

uint64_t getNanos();
uint64_t getMillis();

#endif //TIMER_H
