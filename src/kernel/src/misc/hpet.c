
#include "hpet.h"
#include "acpi.h"
#include "madt.h"

static struct acpi_hpet_table *hpet;
static size_t capabilitiesRegister;
static size_t configRegister;
static size_t interruptStatRegister;
static size_t counterValueRegister;

static uint64_t freqMult = 1; // 1 if period is in excess of 1ns.
static uint64_t freqDiv = 1; // 1 if period is not in excess of 1ns.
static uint64_t nanosOffset;

static unsigned char numComparators;

static uint64_t readR64(size_t address) {
	return *(uint64_t *) address;
}

static void writeR64(size_t address, size_t value) {
	*((uint64_t *) address) = value;
}

static struct hpet_config readConfig() {
	union hpet_config_union cfg;
	cfg.value = readR64(configRegister);
	return cfg.fields;
}

static void writeConfig(struct hpet_config fields) {
	union hpet_config_union cfg;
	cfg.fields = fields;
	writeR64(configRegister, cfg.value);
}

static struct hpet_capabilities readCapabilities() {
	union hpet_capabilities_union cfg;
	cfg.value = readR64(capabilitiesRegister);
	return cfg.fields;
}

static struct hpet_timer_config readTimerConfig(size_t index) {
	size_t address = 0x0100 + 0x20 * index;
	union hpet_timer_config_union cfg;
	cfg.value = readR64(address);
	return cfg.fields;
}

static void writeTimerConfig(size_t index, struct hpet_timer_config fields) {
	size_t address = 0x0100 + 0x20 * index;
	union hpet_timer_config_union cfg;
	cfg.fields = fields;
	writeR64(address, cfg.value);
}

static uint64_t readTimerValue(size_t index) {
	size_t address = 0x0108 + 0x20 * index;
	return readR64(address);
}

static void writeTimerValue(size_t index, uint64_t value) {
	size_t address = 0x0108 + 0x20 * index;
	writeR64(address, value);
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
	struct hpet_config cfg = readConfig();
	cfg.enabled = 0;
	writeConfig(cfg);
	// Calculate frequency multiplier.
	struct hpet_capabilities cap = readCapabilities();
	if (cap.clockPeriod >= 1000000) { // In excess of one nanosecond.
		freqMult = 1;
		freqDiv  = cap.clockPeriod / 1000000;
	} else { // Not in excess of one nanosecond.
		freqMult = cap.clockPeriod / 1000000;
		freqDiv  = 1;
	}
	// Get other info.
	numComparators = cap.numTimers + 1;
	// Set up all the comparators.
	for (size_t i = 0; i < numComparators; i++) {
		struct hpet_timer_config tcfg = readTimerConfig(i);
		tcfg.useInterrupts = 0;
		writeTimerConfig(i, tcfg);
	}
	// Setup MADT stuffs for HPET.
	madtSetup(0);
	// Enable the main counter.
	nanosOffset = 0;
	nanosOffset = hpet_getNanos();
	cfg.enabled = 1;
	writeConfig(cfg);
	// Quick, the watchdog.
	hpet_setTimer(0, 0, 1000000);
	return 1;
}

uint64_t hpet_getNanos() {
	return readR64(counterValueRegister) / freqMult * freqDiv - nanosOffset;
}

void hpet_setTimer(uint8_t num, uint8_t doPeriodic, uint64_t nanos) {
	asm volatile ("cli");
	struct hpet_timer_config tcfg = readTimerConfig(num);
	tcfg.useInterrupts = 0;
	tcfg.interruptNum = 0 /*???*/;
	tcfg.usePeriodic = 0;
	writeTimerConfig(num, tcfg);
	nanos += hpet_getNanos() + 2;
	writeTimerValue(num, nanos * freqMult / freqDiv);
	tcfg.useInterrupts = 1;
	writeTimerConfig(num, tcfg);
	writeR64(interruptStatRegister, 1 << num);
	asm volatile ("sti");
}
