#include <sys/asm.h>

#include "isr.h"
#include "idt.h"
#include "kio.h"
#include "panic.h"

// Reference: https://support.microsoft.com/en-us/kb/117389
// Note: reference refers to FPU as 'coprocessor'

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
static ISR_DEF_HANDLER(isr_general_protection_fault);
static ISR_DEF_HANDLER(isr_page_fault);
static ISR_DEF_HANDLER(isr_fpu_error);

INLINE int __set_handler(int isrnum, void (*handler)(void))
{
    return idt_set_entry(isrnum, handler, 0x8, IDT_PRESENT | IDT_PRIVILEGE_0 |
        IDT_GATE_INTERRUPT_32);
}

INLINE int __remove_handler(int isrnum)
{
    return idt_set_entry(isrnum, 0, 0, 0);
}

int isr_init(void)
{
    int result = 0;

    // CPU Exceptions (some omitted):
    result |= __set_handler(0x00, ISR_HANDLER(isr_divide_error));
    // 0x01 Single-step/Debug (Trap Gate)
    result |= __set_handler(0x02, ISR_HANDLER(isr_nonmaskable_interrupt));
    // 0x03 Breakpoint (Trap Gate)
    // 0x04 Overflow (Trap Gate)
    result |= __set_handler(0x05, ISR_HANDLER(isr_bounds_check));
    result |= __set_handler(0x06, ISR_HANDLER(isr_invalid_opcode));
    result |= __set_handler(0x07, ISR_HANDLER(isr_fpu_unavailable));
    result |= __set_handler(0x08, ISR_HANDLER(isr_double_fault));
    result |= __set_handler(0x09, ISR_HANDLER(isr_fpu_segment_overrun));
    result |= __set_handler(0x0a, ISR_HANDLER(isr_invalid_tss));
    result |= __set_handler(0x0b, ISR_HANDLER(isr_segment_not_present));
    result |= __set_handler(0x0c, ISR_HANDLER(isr_stack_exception));
    result |= __set_handler(0x0d, ISR_HANDLER(isr_general_protection_fault));
    result |= __set_handler(0x0e, ISR_HANDLER(isr_page_fault));
    // 0xf Reserved by Intel
    result |= __set_handler(0x10, ISR_HANDLER(isr_fpu_error));

    if (result) {
        kprintf("isr: failed to register one or more cpu isr handlers\n");
        return 1;
    }

    kprintf("isr: registered cpu isr handlers\n");
    return 0;
}

int isr_set_handler(int isrnum, void (*handler)(void))
{
    if (handler) {
        return __set_handler(isrnum, handler);
    } else {
        return __remove_handler(isrnum);
    }
}

void isr_divide_error(struct isr_params params)
{
    paniccs(params.cs, "cpu divide error\n");
}

void isr_nonmaskable_interrupt(struct isr_params params)
{
    paniccs(params.cs, "cpu non-maskable hardware interrupt\n");
}

void isr_bounds_check(struct isr_params params)
{
    paniccs(params.cs, "cpu bounds limit exceeded\n");
}

void isr_invalid_opcode(struct isr_params params)
{
    paniccs(params.cs, "cpu invalid opcode\n");
}

void isr_fpu_unavailable(struct isr_params params)
{
    paniccs(params.cs, "cpu fpu unavailable\n");
}

void isr_double_fault(struct isr_params params)
{
    paniccs(params.cs, "cpu double-fault\n");
}

void isr_fpu_segment_overrun(struct isr_params params)
{
    paniccs(params.cs, "cpu fpu segment overrun\n");
}

void isr_invalid_tss(struct isr_params params)
{
    paniccs(params.cs, "cpu invalid TSS\n");
}

void isr_segment_not_present(struct isr_params params)
{
    paniccs(params.cs, "cpu segment not present\n");
}

void isr_stack_exception(struct isr_params params)
{
    paniccs(params.cs, "cpu stack exception\n");
}

void isr_general_protection_fault(struct isr_params params)
{
    paniccs(params.cs, "cpu general protection fault\n");
}

void isr_page_fault(struct isr_params params)
{
    paniccs(params.cs, "cpu page fault\n");
}

void isr_fpu_error(struct isr_params params)
{
    paniccs(params.cs, "cpu fpu error\n");
}
