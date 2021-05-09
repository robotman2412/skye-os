
#include "timer.h"
#include "acpi.h"
#include "hpet.h"
//#include "pit.h"

static char useHpet;

void timerSetup() {
	if (!isAcpiV2) {
		useHpet = 0;
	} else {
		useHpet = hpetSetup();
		if (!useHpet) {
			//pitSetup();
		}
	}
	if (useHpet) {
		logk("Using HPET for timers.\n");
	} else {
		logk("Using PIT for timers.\n");
	}
}

void resetWatchdog() {
	
}
