
#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>
#include <stddef.h>
#include "stivale2.h"

#define STRUCT_RSDP_SIGNATURE "RSP PTR "
#define STRUCT_ACPI_STD_SIGNATURE "FADT"

struct rsdp_descriptor {
	char signature[8];
	uint8_t checksum;
	char oemId[6];
	uint8_t revision;
	uint32_t rsdtAddress;
} __attribute__ ((packed));

struct ext_rsdp_descriptor {
	char signature[8];
	uint8_t checksum;
	char oemId[6];
	uint8_t revision;
	uint32_t rsdtAddress;

	uint32_t length;
	uint64_t xdstAddress;
	uint8_t extChecksum;
	uint8_t reserved[3];
} __attribute__ ((packed));

struct acpi_std_header {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oemId[6];
	char oemTableId[8];
	uint32_t oemRevision;
	uint32_t creatorId;
	uint32_t creatorRevision;
} __attribute__ ((packed));

extern char isAcpiV2;

void confuseAcpi(struct stivale2_struct_tag_rsdp *rsdpTag);

void *getAcpiTable(char *signature);

#endif //ACPI_H
