#include <kernel/compiler.h>
#include <kernel/types.h>

#include "gdt.h"

BEGIN_PACK struct gdt_entry {
    // TODO
} END_PACK;

// 48-bit GDT descriptor, for use with lgdt and sgdt
BEGIN_PACK struct gdt_descriptor {
    u16                 size;
    struct gdt_entry    *base;
} END_PACK;
