
#ifndef HPET_H
#define HPET_H

#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "acpi.h"

struct hpet_capabilities {
	uint8_t revision : 8;
	uint8_t numTimers : 5;
	uint8_t does64 : 1;
	uint8_t reserved : 1;
	uint8_t canDoLegacy : 1;
	uint16_t vendorId : 16;
	uint32_t clockPeriod : 32; // Clock tick in femtoseconds.
} __attribute__((packed));

struct hpet_config {
	uint8_t enabled : 1;
	uint8_t useLegacy : 1;
	uint64_t reserved : 62;
} __attribute__((packed));

struct hpet_timer_config {
	uint8_t reserved0 : 1;
	uint8_t interruptType : 1; // what?
	uint8_t useInterrupts : 1;
	uint8_t usePeriodic : 1;
	uint8_t doesPeriodic : 1;
	uint8_t does64 : 1;
	uint8_t setPeriod : 1; // uhh?
	uint8_t rederved1 : 1;
	uint8_t use32 : 1; // Reverts to 32-bit mode.
	uint8_t interruptNum : 5; // Interrupt vector to use.
	uint8_t useFSB : 1;
	uint8_t doesFSB : 1;
	uint16_t reserved2 : 16;
	uint32_t supportedInterruptNumbers; // For each supported I/O APIC interrupt line, the corresponding bit is set.
} __attribute__((packed));

struct hpet_address {
    uint8_t address_space_id;    // 0 - system memory, 1 - system I/O
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
} __attribute__((packed));

struct acpi_hpet_table {
	struct acpi_std_header header; // Signature must equal "HPET".
	
    uint8_t hardwareRevision;
    uint8_t comparatorCount:5;
    uint8_t counterSize:1;
    uint8_t reserved:1;
    uint8_t legacyReplacement:1;
    uint16_t pciVendorId;
	struct hpet_address address;
    uint8_t hpetNum;
    uint16_t minimumTick;
    uint8_t pageProtection;
} __attribute__ ((packed));

char hpetSetup();

uint64_t hpet_getNanos();

#endif //HPET_H
