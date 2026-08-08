ENTRY(_start)

SECTIONS
{
    /*
     * Low-RAM layout for the ROM monitor.
     *
     * The full unrelocated 68020 vector table occupies
     * $00000000..$000003ff.
     *
     * Recommended pairings:
     *   32 KiB RAM, monitor only:
     *     __ram_base  = 0x00000400;
     *     __stack_top = 0x00008000;
     *
     *   256 KiB RAM, boot-capable:
     *     __ram_base  = 0x00030000;
     *     __stack_top = 0x00040000;
     */
    __ram_base = 0x00030000;
    __stack_top = 0x00040000;

    . = 0x00800000;

    .text :
    {
        __text_start = .;
        *(.text)
        *(CODE)
        *(CODE*)
        *(.rodata)
        *(.rodata*)
        __text_end = .;
    }

    __data_load = .;
    . = __ram_base;

    .data : AT(__data_load)
    {
        __data_start = .;
        ___data_start = .;
        *(DATA)
        *(.data)
        *(.data*)
        __data_end = .;
    }

    .bss :
    {
        __bss_start = .;
        *(BSS)
        *(.bss)
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }
}
