OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0;
    .main (1M) : { *(.main)   }
    .text      : { *(.text)   }
    .data      : { *(.data)   }
    .rodata    : { *(.rodata) }
    .bss       : { *(.bss)    }
}
