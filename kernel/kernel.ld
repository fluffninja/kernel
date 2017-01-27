STARTUP(start.bin)
SECTIONS
{
    . = 0;
    .text (1M) : {
        *(.text)
    }
    .data : {
        *(.data)
    }
    .rodata : {
        *(.rodata)
    }
    .bss : {
        *(.bss)
    }
}
