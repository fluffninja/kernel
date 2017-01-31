#ifndef _INC_IDT
#define _INC_IDT 1

#define IDT_MAX 256

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

int idt_init(void);
bool idt_is_valid_index(int index);
bool idt_has_entry(int index);
int idt_set_entry(int index, void (*handler)(void), int selector, int flags);

#endif /* _INC_IDT */
