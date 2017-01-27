#include <kernel/klog.h>

#include "idt.h"
#include "../kutil.h"

#define IDT_SIZE    256

static struct idt_entry s_idt[IDT_SIZE];

static INLINE int __idt_is_valid_index_impl(int index)
{
    return !((index < 0) || (index >= (int) ARRLEN(s_idt)));
}

int idt_init(void)
{
    kzeromem(s_idt, sizeof(s_idt));

    struct idt_descriptor descriptor = { sizeof(s_idt), s_idt };

    ASM_VOLATILE("lidt [eax]"::"a"(&descriptor));

    klog_printf("idt: loaded with size %d at %p\n", ARRLEN(s_idt), s_idt);

    return 0;
}

int idt_is_valid_index(int index)
{
    return __idt_is_valid_index_impl(index);
}

int idt_has_entry(int index)
{
    if (!__idt_is_valid_index_impl(index)) {
        return 1;
    }

    struct idt_entry entry = s_idt[index];
    return (entry.flags & IDT_PRESENT);
}

int idt_set_entry(int index, void (*handler)(void), int selector, int flags)
{
    if (!__idt_is_valid_index_impl(index)) {
        return 1;
    }

    struct idt_entry entry = IDT_DEF_ENTRY(handler, selector, flags);
    s_idt[index] = entry;

    return 0;
}

