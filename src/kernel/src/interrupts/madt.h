
#ifndef IOAPIC_H
#define IOAPIC_H

#include "logging.h"
#include "acpi.h"

// Single processor and it's local interrupt controller.
#define MADT_TYPE_PROC_LOCAL_APIC 0x00
// The I/O APIC.
#define MADT_TYPE_IOAPIC 0x01
// Mapping IRQ to interrupts.
#define MADT_TYPE_INTERRUPT_OVERRIDE 0x02
// Not quite sure.
#define MADT_TYPE_NMI 0x04
// Local APIC address override.
#define MADT_TYPE_LOCAL_APIC_OVERRIDE 0x05

struct acpi_madt_table {
	struct acpi_std_header header;
	
	uint32_t localApicAddress;
	uint32_t flags;
};

void madtSetup(char usePit);

#endif //IOAPIC_H
