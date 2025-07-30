/* kernel/kernel.ld - Linker script for Kyro OS kernel */

ENTRY(_start)

SECTIONS
{
    . = 0x1000;

    .text : ALIGN(4096)
    {
        *(.text.start)
        *(.text)
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