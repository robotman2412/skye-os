
#ifdef HPET_H
#define HPET_H

#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "acpi.h"

struct hpet_address {
    uint8_t address_space_id;    // 0 - system memory, 1 - system I/O
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
} __attribute__((packed));

struct acpi_hpet_table {
	char signature[4]; // Must equal "HPET"
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oemId[6];
	char oemTableId[8];
	uint32_t oemRevision;
	uint32_t creatorId;
	uint32_t creatorRevision;
	
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

#endif //HPET_H
