
#include "hpet.h"
#include "acpi.h"

static int numHpets;
static struct acpi_hpet_table **hpetTables;

char hpetSetup() {
	numHpets = countAcpiTables("HPET");
	hpetTables = (struct acpi_hpet_table **) kalloc(sizeof(size_t) * numHpets);
	getAcpiTables("HPET", (void **) hpetTables);
	// TODO
	return 0;
}
