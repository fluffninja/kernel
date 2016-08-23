OUTPUT_FORMAT(binary)
STARTUP(bootstrap.o)
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
