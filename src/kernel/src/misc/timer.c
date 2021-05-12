
#include "timer.h"
#include "acpi.h"
#include "hpet.h"
#include "pit.h"

static char isSetup = 0;
static char useHpet;

void timerSetup() {
	useHpet = hpetSetup();
	if (useHpet) {
		logk("Using HPET for timers.\n");
	} else {
		pitSetup();
		logk("Using PIT for timers.\n");
	}
}

void resetWatchdog() {
	
}

uint64_t getNanos() {
	return isSetup ? (useHpet ? hpet_getNanos() : 0) : 0;
}

uint64_t getMillis() {
	return getNanos() / 1000000;
}
