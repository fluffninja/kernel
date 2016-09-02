#include <sys/asm.h>

#include "idt.h"
#include "isr.h"
#include "kstring.h"

static ISR_DEF_HANDLER(isr_divide_error);
static ISR_DEF_HANDLER(isr_nonmaskable_interrupt);
static ISR_DEF_HANDLER(isr_bounds_check);
static ISR_DEF_HANDLER(isr_invalid_opcode);
static ISR_DEF_HANDLER(isr_fpu_unavailable);
static ISR_DEF_HANDLER(isr_double_fault);
static ISR_DEF_HANDLER(isr_fpu_segment_overrun);
static ISR_DEF_HANDLER(isr_invalid_tss);
static ISR_DEF_HANDLER(isr_segment_not_present);
static ISR_DEF_HANDLER(isr_stack_exception);
static ISR_DEF_HANDLER(isr_general_protection_violation);
static ISR_DEF_HANDLER(isr_page_fault);
static ISR_DEF_HANDLER(isr_fpu_error);

int isr_init(void)
{
    // CPU Exceptions (some omitted):
    isr_set_handler(0x00, ISR_HANDLER(isr_divide_error));
    // 0x01 Single-step/Debug Trap Gate
    isr_set_handler(0x02, ISR_HANDLER(isr_nonmaskable_interrupt));
    // 0x03 Breakpoint Trap Gate
    isr_set_handler(0x05, ISR_HANDLER(isr_bounds_check));
    isr_set_handler(0x06, ISR_HANDLER(isr_invalid_opcode));
    isr_set_handler(0x07, ISR_HANDLER(isr_fpu_unavailable));
    isr_set_handler(0x08, ISR_HANDLER(isr_double_fault));
    isr_set_handler(0x09, ISR_HANDLER(isr_fpu_segment_overrun));
    isr_set_handler(0x0a, ISR_HANDLER(isr_invalid_tss));
    isr_set_handler(0x0b, ISR_HANDLER(isr_segment_not_present));
    isr_set_handler(0x0c, ISR_HANDLER(isr_stack_exception));
    isr_set_handler(0x0d, ISR_HANDLER(isr_general_protection_violation));
    isr_set_handler(0x0e, ISR_HANDLER(isr_page_fault));
    // 0xf Reserved
    isr_set_handler(0x10, ISR_HANDLER(isr_fpu_error));    

    kprintf("isr: registered isr handlers for cpu\n");

    return 0;
}

int isr_set_handler(int index, void (*handler)(void))
{
    if (!handler) {
        return idt_set_entry(index, handler, 0, 0x8);
    } else {
        return idt_set_entry(index, handler, 0x8,
            IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_GATE_INTERRUPT_32);
    }
}

// TODO: make this just return a string, printable by kvsprintf (or whatever
// the heck it's called).
void print_register_set(struct register_set regset)
{
    kprintf(
        " eax=%x ebx=%x ecx=%x edx=%x\n"
        " esi=%x edi=%x ebp=%x esp=%x\n",
        regset.a, regset.b, regset.c, regset.d,
        regset.si, regset.di, regset.bp, regset.sp);
}    

// Reference: https://support.microsoft.com/en-us/kb/117389
// Note: 'coprocessor' is another term for FPU.

void isr_divide_error(struct register_set regset)
{
    kprintf("cpu: divide error\n");
    print_register_set(regset);
    while (1);
}

void isr_nonmaskable_interrupt(struct register_set regset)
{
    kprintf("cpu: non-maskable hardware interrupt\n");
    print_register_set(regset);
    while (1);
}

void isr_bounds_check(struct register_set regset)
{
    kprintf("cpu: bounds limit exceeded\n");
    print_register_set(regset);
    while (1);
}

void isr_invalid_opcode(struct register_set regset)
{
    kprintf("cpu: invalid opcode\n");
    print_register_set(regset);
    while (1);
}

void isr_fpu_unavailable(struct register_set regset)
{
    kprintf("cpu: fpu unavailable\n");
    print_register_set(regset);
    while (1);
}

void isr_double_fault(struct register_set regset)
{
    kprintf("cpu: double-fault\n");
    print_register_set(regset);
    while (1);
}

void isr_fpu_segment_overrun(struct register_set regset)
{
    kprintf("cpu: fpu segment overrun\n");
    print_register_set(regset);
    while (1);
}

void isr_invalid_tss(struct register_set regset)
{
    kprintf("cpu: invalid TSS\n");
    print_register_set(regset);
    while (1);
}

void isr_segment_not_present(struct register_set regset)
{
    kprintf("cpu: segment not present\n");
    print_register_set(regset);
    while (1);
}

void isr_stack_exception(struct register_set regset)
{
    kprintf("cpu: stack exception\n");    
    print_register_set(regset);
    while (1);
}

void isr_general_protection_violation(struct register_set regset)
{
    kprintf("cpu: general protection violation\n");
    print_register_set(regset);
    while (1);
}

void isr_page_fault(struct register_set regset)
{
    kprintf("cpu: page fault\n");
    print_register_set(regset);
    while (1);
}

void isr_fpu_error(struct register_set regset)
{
    kprintf("cpu: fpu error\n");
    print_register_set(regset);
    while (1);
}
