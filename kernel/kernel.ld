OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0;
    .main (0x8000) : { *(.main)   }
    .text          : { *(.text)   }
    .data          : { *(.data)   }
    .rodata        : { *(.rodata) }
    .bss           : { *(.bss)    }
}
