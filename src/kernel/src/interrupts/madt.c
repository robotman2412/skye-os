
#include "madt.h"
#include "ports.h"

static struct acpi_madt_table *madt;

void disablePic() {
	// Masks out all PIC interrupts.
	outb(0x21, 0xff);
	outb(0xa1, 0xff);
	// Wait for the PIC.
	outb(0x80, 0x00);
}

void madtSetup(char usePit) {
	if (!usePit) {
		// Disable the PIC.
		disablePic();
		debugk("PIC disabled.\n");
	} else {
		errk("PIT and PIC support is not finished!\n");
	}
	// Get the correct ACPI table.
	madt = getAcpiTable("MADT");
}
