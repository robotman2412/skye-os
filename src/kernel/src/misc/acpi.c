
#include "acpi.h"
#include "kernel.h"
#include "stivale2.h"


char isAcpiV2;
static char isSetup = 0;
static struct ext_rsdp_descriptor *rsdp;

void confuseAcpi(struct stivale2_struct_tag_rsdp *rsdpTag) {
	rsdp = (struct ext_rsdp_descriptor *) rsdpTag->rsdp;
	if (!rsdp) {
		warnk("Got RSDP struct, but not RSDP.");
		return;
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

size_t countAcpiTables(char *signature) {
	if (!isSetup) {
		warnk("ACPI is not set up!\n");
		return NULL;
	} else if (isAcpiV2) {
		struct acpi_std_header *headers = ((void *) rsdp) + sizeof(struct rsdp_descriptor);
	} else {
		struct acpi_std_header *header;
		uint32_t *shortAddresses;
		
	}
}

void getAcpiTables(char *signature, void** table) {
	if (!isSetup) {
		warnk("ACPI is not set up!\n");
		return NULL;
	} else if (isAcpiV2) {
		
	} else {
		
	}
}

