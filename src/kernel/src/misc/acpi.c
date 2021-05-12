
#include "acpi.h"
#include "string.h"
#include "kernel.h"
#include "stivale2.h"

char isAcpiV2;
static char isSetup = 0;
static struct ext_rsdp_descriptor *rsdp;

static char chksum(void *ptr, size_t length) {
	uint8_t *verify = (uint8_t *) ptr;
	uint16_t chksum = 0;
	for (int i = 0; i < length; i++) {
		chksum += verify[i];
	}
	return !(chksum & 0xff);
}

void confuseAcpi(struct stivale2_struct_tag_rsdp *rsdpTag) {
	rsdp = (struct ext_rsdp_descriptor *) rsdpTag->rsdp;
	if (!rsdp) {
		warnk("Got RSDP struct, but not RSDP.");
		return;
	}
	if (!chksum(rsdp, sizeof(struct rsdp_descriptor))) {
		warnk("RSDP is invalid!\n");
		kpanic();
	}
	if (rsdp->revision < 2) {
		logk("Detected RSDP version one.\n");
		isAcpiV2 = 0;
	} else {
		logk("Detected RSDP version 2+\n");
		isAcpiV2 = 1;
	}
	isSetup = 1;
}

void *getAcpiTable(char *signature) {
	if (!isSetup) {
		warnk("ACPI is not set up!\n");
		return NULL;
	} else if (isAcpiV2) {
		struct acpi_std_header *rsdt = (struct acpi_std_header *) (uint64_t) rsdp->rsdtAddress;
		struct acpi_std_header *header;
		uint64_t *addresses = ((void *) rsdt) + sizeof(struct acpi_std_header);
	} else {
		struct acpi_std_header *rsdt = (struct acpi_std_header *) (uint64_t) rsdp->rsdtAddress;
		struct acpi_std_header *header;
		uint32_t *shortAddresses = ((void *) rsdt) + sizeof(struct acpi_std_header);
		size_t numAddresses = (rsdt->length - sizeof(struct acpi_std_header)) / sizeof(uint32_t);
		for (size_t i = 0; i < numAddresses; i++) {
			header = (struct acpi_std_header *) (uint64_t) shortAddresses[i];
			if (!chksum((void *) header, header->length)) {
				char sig[5];
				memcpy(sig, header->signature, 4);
				sig[4] = 0;
				char *msg = kalloc(42);
				strcpy(msg, "ACPI table ");
				strcat(msg, sig);
				strcat(msg, " has an invalid checksum!\n");
				warnk(msg);
				kfree(msg);
			} else if (!strncmp(signature, header->signature, 4)) {
				return (void *) header;
			}
		}
	}
	return NULL;
}

