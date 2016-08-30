#include <sys/asm.h>
#include <stddef.h>
#include <stdint.h>

#include "ps2.h"
#include "con.h"
#include "pic.h"
#include "kstring.h"

// Dumb waiting function
void dumb_wait(int mult)
{
    static volatile int a;
    volatile int *p = &a;
    for (int i = 0; i < mult * 1000000; ++i) {
        a = *p;
        *p = a;
    }
}


static struct idt_entry idt[256] = { 0 };

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

    struct idt_entry entry = DEF_IDT_ENTRY(handler, selector, flags);
    idt[index] = entry;

    return 0;
}

int idt_disable(int index)
{
    if (!idt_is_valid_index(index)) {
        return 1;
    }

    struct idt_entry entry = idt[index];
    entry.flags &= ~IDT_PRESENT;
    idt[index] = entry;

    return 0;
}

int idt_enable(int index)
{
    if (!idt_is_valid_index(index)) {
        return 1;
    }

    struct idt_entry entry = idt[index];
    entry.flags |= IDT_PRESENT;
    idt[index] = entry;

    return 0;
}

void int_double_fault(void)
{
    ASM_VOLATILE("pusha");
    kprintf("Double-fault\n");
    while (1);
    ASM_VOLATILE("popa; leave; iret");
}

int idt_init(void)
{
    idt_set_entry(8, &int_double_fault, 0x08, IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_GATE_INTERRUPT_32);
    idt_set_entry(8, &int_double_fault, 0x08, IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_GATE_INTERRUPT_32);

    struct idt_descriptor descriptor = { sizeof(idt), idt };

    ASM_VOLATILE("lidt [eax]"::"a"(&descriptor));

    kprintf("IDT loaded: %u entries at %p\n", ARRLEN(idt), idt);

    return 0;
}

void CDECL NO_RETURN 
kmain(void)
{ 
    con_init();
    pic_init();
    idt_init();
    sti();

    while (1);
}
