#ifndef _INC_IDT
#define _INC_IDT 1

#include <compiler.h>
#include <stdint.h>

// IDT Entry Flags
#define IDT_PRESENT             0x80 // Interrupt entry should be used
#define IDT_PRIVILEGE_0         0x00 // Highest ring that can use interrupt
#define IDT_PRIVILEGE_1         0x20 // ^
#define IDT_PRIVILEGE_2         0x40 // ^
#define IDT_PRIVILEGE_3         0x60 // ^
#define IDT_GATE_TASK_32        0x05 // This is a 32-bit task gate
#define IDT_GATE_INTERRUPT_16   0x06 // This io a 16-bit interrupt gate
#define IDT_GATE_TRAP_16        0x07 // This is a 16-bit task gate
#define IDT_GATE_INTERRUPT_32   0x0e // This is a 32-bit interrupt gate
#define IDT_GATE_TRAP_32        0x0f // This is a 32-bit trap gate

BEGIN_PACK struct idt_entry {
    uint16_t handler_offset_00_15;  // Offset of handler code in GDT segment
    uint16_t gdt_handler_selector;  // GDT segment - as table selector
    uint8_t  _reserved0;            // Reserved byte
    uint8_t  flags;                 // Flags detailing type and attributes
    uint16_t handler_offset_16_32;  // Upper word of handler offset
} END_PACK;

#define IDT_DEF_ENTRY(HANDLER_OFFSET, GDT_SELECTOR, FLAGS)      \
    {                                                           \
        (uint16_t) (((uint32_t) (HANDLER_OFFSET)) & 0xffff),    \
        (uint16_t) (GDT_SELECTOR),                              \
        0,                                                      \
        (uint8_t) (FLAGS),                                      \
        (uint16_t) (((uint32_t) (HANDLER_OFFSET)) >> 16),       \
    }

// 48-bit IDT descriptor, for use with lidt and sidt
BEGIN_PACK struct idt_descriptor {
    uint16_t            size;
    struct idt_entry    *base;
} END_PACK;

int idt_init(void);
int idt_is_valid_index(int index);
int idt_has_entry(int index);
int idt_set_entry(int index, void (*handler)(void), int selector, int flags);

#endif /* _INC_IDT */
