
#include "hpet.h"
#include "acpi.h"

static struct acpi_hpet_table volatile *hpet;
static size_t volatile capabilitiesRegister;
static size_t volatile configRegister;
static size_t volatile interruptStatRegister;
static size_t volatile counterValueRegister;

static uint64_t volatile freqMult = 1; // 1 if period is in excess of 1ns.
static uint64_t volatile freqDiv = 1; // 1 if period is not in excess of 1ns.
static uint64_t volatile nanosOffset;

static inline uint64_t readR64(size_t address) {
	uint64_t out;
	asm volatile ("mov %0, %1" : "=r" (out) : "m" (*(uint64_t *) address));
	return out;
}

static inline void writeR64(size_t address, size_t value) {
	asm volatile ("mov %0, %1" :: "m" (*(uint64_t *) address), "r" (value));
}

static inline struct hpet_config readConfig() {
	uint64_t v = readR64(configRegister);
	return *(struct hpet_config *) &v;
}

static inline void writeConfig(struct hpet_config cfg) {
	writeR64(configRegister, *(uint64_t *) &cfg);
}

static inline struct hpet_capabilities readCapabilities() {
	uint64_t v = readR64(configRegister);
	return *(struct hpet_capabilities *) &v;
}

char hpetSetup() {
	// Detect HPET.
	hpet = getAcpiTable("HPET");
	if (!hpet) {
		return 0;
	}
	// Calculate all the addresses.
	capabilitiesRegister	= hpet->address.address + 0x0000;
	configRegister			= hpet->address.address + 0x0010;
	interruptStatRegister	= hpet->address.address + 0x0020;
	counterValueRegister	= hpet->address.address + 0x00f0;
	// Disable the HPET.
	struct hpet_config volatile cfg = readConfig();
	cfg.enabled = 0;
	writeConfig(cfg);
	// Calculate frequency multiplier.
	struct hpet_capabilities volatile cap = readCapabilities();
	if (cap.clockPeriod >= 1000000) { // In excess of one nanosecond.
		//freqMult = 1;
		//freqDiv  = cap.clockPeriod / 1000000;
	} else { // Not in excess of one nanosecond.
		//freqMult = cap.clockPeriod / 1000000;
		//freqDiv  = 1;
	}
	logkf("HPET period is %ld femtoseconds.\n", cap.clockPeriod);
	// Set up all the comparators.
	return 1;
}

uint64_t hpet_getNanos() {
	return readR64(counterValueRegister) / freqMult * freqDiv - nanosOffset;
}
