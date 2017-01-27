#ifndef _INC_GDT
#define _INC_GDT 1

#include <stdint.h>

#include <kernel/compiler.h>

BEGIN_PACK struct gdt_entry {
} END_PACK;

// 48-bit GDT descriptor, for use with lgdt and sgdt
BEGIN_PACK struct gdt_descriptor {
    uint16_t            size;
    struct gdt_entry    *base;
} END_PACK;

#endif /* _INC_GDT */
