/* kernel/kernel.ld - Linker script for Kyro OS kernel */

ENTRY(_start)

SECTIONS
{
    . = 0x1000;

    /* Ensure the entry point is at the very beginning */
    .text.start : ALIGN(4096)
    {
        *(.text.start)
    }

    .text : ALIGN(4096)
    {
        *(.text)
    }
    
    .rodata : ALIGN(4096)
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data : ALIGN(4096)
    {
        *(.data)
    }

    .bss : ALIGN(4096)
    {
        sbss = .;
        *(.bss)
        *(COMMON)
        ebss = .;
    }

    kernel_end = .;
}