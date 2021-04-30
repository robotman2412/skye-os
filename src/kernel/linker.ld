
/* 
 * Skye OS: linker.ld
 * (c) J. Scheffers, who cares
 * Do whatever you want with it
 */

/* Tell the linker that we want the symbol _start to be our entry point */
ENTRY(_start)
 
SECTIONS
{
    /* Address offset, higher half, 2MB above addr 0. */
    . = 0xffffffff80200000;
 
    /* Never discard this, required by stivale2. */
    .stivale2hdr : {
        KEEP(*(.stivale2hdr))
    }
 
    /* Standard section types. */
    .text : {
        *(.text*)
    }
 
    .rodata : {
        *(.rodata*)
    }
 
    .data : {
        *(.data*)
    }
 
    .bss : {
        *(COMMON)
        *(.bss*)
    }
}
