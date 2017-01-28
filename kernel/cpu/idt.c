#include <kernel/kernel.h>
#include <kernel/compiler.h>
#include <kernel/types.h>
#include <kernel/klog.h>
#include <kernel/kerror.h>

#include "idt.h"

BEGIN_PACK
struct idt_entry {
    u16 handler_offset_low_word;    // Low word of handler offset within the
                                    // given GDT segment.
    u16 gdt_handler_selector;       // GDT selector for the GDT segment
                                    // containing the given handler.
    u8 reserved;
    u8 flags;                       // Flags for this IDT entry.
    u16 handler_offset_high_word;   // High word of handler offset.
}
END_PACK;

BEGIN_PACK
struct idt_descriptor {
    u16 size;                   // Number of entries in the IDT.
    struct idt_entry *base;     // Address of the first entry of the IDT.
}
END_PACK;

static struct idt_entry s_idt[IDT_MAX];

static int initialsie_entry(struct idt_entry *entry, void *handler_offset,
    int gdt_selector, int flags)
{
    if (!entry) {
        return KERROR_ARG_NULL;
    }

    KZEROMEM(entry, sizeof(struct idt_entry));

    entry->handler_offset_low_word = (u16) (((int) handler_offset) & 0xffff);
    entry->handler_offset_high_word = (u16) (((int) handler_offset) >> 16);
    entry->gdt_handler_selector = (u16) gdt_selector;
    entry->flags = (u8) flags;

    return 0;
}

static void load_descriptor(int size, void *base)
{
    struct idt_descriptor descriptor;
    descriptor.size = size;
    descriptor.base = base;
    ASM_VOLATILE(
        "lidt [eax]"::
        "a"(&descriptor)
    );
}

int idt_init(void)
{
    // Zero out the IDT
    KZEROMEM(s_idt, sizeof(s_idt));

    load_descriptor((int) sizeof(s_idt), s_idt);

    klog_printf("idt: loaded at %p with %d entries (%zuB)\n", s_idt,
        ARRLEN(s_idt), sizeof(s_idt));

    return 0;
}

static INLINE bool _is_valid_index(int index)
{
    return ((index >= 0) && (index <= IDT_MAX));
}

bool idt_is_valid_index(int index)
{
    return _is_valid_index(index);
}

bool idt_has_entry(int index)
{
    if (!_is_valid_index(index)) {
        return false;
    }

    struct idt_entry entry = s_idt[index];

    return ((bool) (entry.flags & IDT_PRESENT));
}

int idt_set_entry(int index, void (*handler)(void), int selector, int flags)
{
    if (!_is_valid_index(index)) {
        return KERROR_ARG_OUT_OF_RANGE;
    }

    struct idt_entry *entry_ptr = &(s_idt[index]);

    if (initialsie_entry(entry_ptr, (void *) handler, selector, flags)) {
        return KERROR_UNSPECIFIED;
    }

    return 0;
}

