#include "idt.h"
#include "kstring.h"

static struct idt_entry idt[256] = { 0 };

int idt_init(void)
{
    struct idt_descriptor descriptor = { sizeof(idt), idt };
    ASM_VOLATILE("lidt [eax]"::"a"(&descriptor));
    kprintf("idt: loaded with size %d at %p\n", ARRLEN(idt), idt);

    return 0;
}

int idt_is_valid_index(int index)
{    
    if (index < 0 || index >= (int) ARRLEN(idt)) {
        return 0;
    } else {
        return 1;
    }
}

int idt_set_entry(int index, void (*handler)(void), int selector, int flags)
{
    if (!idt_is_valid_index(index)) {
        return 1;
    }

    struct idt_entry entry = IDT_DEF_ENTRY(handler, selector, flags);
    idt[index] = entry;

    return 0;
}

