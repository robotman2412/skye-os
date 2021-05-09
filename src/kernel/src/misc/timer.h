
#ifdef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "hpet.h"
//#include "pit.h"

void timerSetup();
void resetWatchdog();

#endif //TIMER_H
