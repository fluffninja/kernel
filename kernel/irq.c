#include <sys/asm.h>
#include <stdint.h>
#include <stddef.h>

#include "irq.h"
#include "isr.h"
#include "pic.h"
#include "kio.h"
#include "kutil.h"

// Reference for IRQs' respective devices:
// https://en.wikipedia.org/wiki/Interrupt_request_(PC_architecture)

static irq_hook_t irq_hooks[16];

#define DEFAULT_HOOK_FUNC   __irq_default_hook_func

static int DEFAULT_HOOK_FUNC(int irqnum)
{
    // Should irq_done(irqnum) be called for the default handler and/or
    // make this an option, e.g. irq_auto_dismiss_unhooked(int)?
    (void) irqnum;
    return 0;
}

ALWAYS_INLINE int __irq_is_valid_irqnum_impl(int irqnum)
{
    return !((irqnum < 0) || (irqnum >= (int) ARRLEN(irq_hooks)));
}

ALWAYS_INLINE int __irq_has_hook_impl(int irqnum)
{
    return (irq_hooks[irqnum] != DEFAULT_HOOK_FUNC);
}

ALWAYS_INLINE int __irq_call_hook_impl(int irqnum)
{
    return (irq_hooks[irqnum](irqnum));
}

#define IRQ_ISR_HANDLER(IRQNUM)     ISR_HANDLER(irq_##IRQNUM)

#define IRQ_DEF_ISR_HANDLER(IRQNUM)                 \
    __ISR_HOOK_HANDLER_BASE(IRQ_ISR_HANDLER(IRQNUM),\
    {                                               \
        __irq_call_hook_impl(IRQNUM);               \
    });

static IRQ_DEF_ISR_HANDLER(0);
static IRQ_DEF_ISR_HANDLER(1);
static IRQ_DEF_ISR_HANDLER(2);
static IRQ_DEF_ISR_HANDLER(3);
static IRQ_DEF_ISR_HANDLER(4);
static IRQ_DEF_ISR_HANDLER(5);
static IRQ_DEF_ISR_HANDLER(6);
static IRQ_DEF_ISR_HANDLER(7);

static IRQ_DEF_ISR_HANDLER(8);
static IRQ_DEF_ISR_HANDLER(9);
static IRQ_DEF_ISR_HANDLER(10);
static IRQ_DEF_ISR_HANDLER(11);
static IRQ_DEF_ISR_HANDLER(12);
static IRQ_DEF_ISR_HANDLER(13);
static IRQ_DEF_ISR_HANDLER(14);
static IRQ_DEF_ISR_HANDLER(15);

int irq_init(void)
{
    int res = 0;

    kprintf("irq: remapping pic\n");
    if (pic_remap(IRQ_PIC_MASTER_IDT_OFFSET, IRQ_PIC_SLAVE_IDT_OFFSET)) {
        kprintf("irq: irq init failed\n");
        return 1;
    }

    // Set all hooks to the default hook function
    for (size_t i = 0; i < 16; ++i) {
        irq_hooks[i] = DEFAULT_HOOK_FUNC;
    }

    // PIC Master IRQs
    res |= isr_set_handler(IRQ_PIC_MASTER_IDT_OFFSET + 0, IRQ_ISR_HANDLER(0));
    res |= isr_set_handler(IRQ_PIC_MASTER_IDT_OFFSET + 1, IRQ_ISR_HANDLER(1));
    res |= isr_set_handler(IRQ_PIC_MASTER_IDT_OFFSET + 2, IRQ_ISR_HANDLER(2));
    res |= isr_set_handler(IRQ_PIC_MASTER_IDT_OFFSET + 3, IRQ_ISR_HANDLER(3));
    res |= isr_set_handler(IRQ_PIC_MASTER_IDT_OFFSET + 4, IRQ_ISR_HANDLER(4));
    res |= isr_set_handler(IRQ_PIC_MASTER_IDT_OFFSET + 5, IRQ_ISR_HANDLER(5));
    res |= isr_set_handler(IRQ_PIC_MASTER_IDT_OFFSET + 6, IRQ_ISR_HANDLER(6));
    res |= isr_set_handler(IRQ_PIC_MASTER_IDT_OFFSET + 7, IRQ_ISR_HANDLER(7));

    // PIC Slave IRQs
    res |= isr_set_handler(IRQ_PIC_SLAVE_IDT_OFFSET + 0, IRQ_ISR_HANDLER(8));
    res |= isr_set_handler(IRQ_PIC_SLAVE_IDT_OFFSET + 1, IRQ_ISR_HANDLER(9));
    res |= isr_set_handler(IRQ_PIC_SLAVE_IDT_OFFSET + 2, IRQ_ISR_HANDLER(10));
    res |= isr_set_handler(IRQ_PIC_SLAVE_IDT_OFFSET + 3, IRQ_ISR_HANDLER(11));
    res |= isr_set_handler(IRQ_PIC_SLAVE_IDT_OFFSET + 4, IRQ_ISR_HANDLER(12));
    res |= isr_set_handler(IRQ_PIC_SLAVE_IDT_OFFSET + 5, IRQ_ISR_HANDLER(13));
    res |= isr_set_handler(IRQ_PIC_SLAVE_IDT_OFFSET + 6, IRQ_ISR_HANDLER(14));
    res |= isr_set_handler(IRQ_PIC_SLAVE_IDT_OFFSET + 7, IRQ_ISR_HANDLER(15));

    if (res) {
        kprintf("irq: failed to register one or more irq isr handlers\n");
        return 1;
    }

    kprintf("irq: registered irq isr handlers\n");
    return 0;
}

int irq_is_valid_irqnum(int irqnum)
{
    return __irq_is_valid_irqnum_impl(irqnum);
}

int irq_has_hook(int irqnum)
{
    if (__irq_is_valid_irqnum_impl(irqnum)) {
        return __irq_has_hook_impl(irqnum);
    } else {
        return 1;
    }
}

int irq_set_hook(int irqnum, irq_hook_t hookfn)
{
    char fail_str[256];

    if (__irq_is_valid_irqnum_impl(irqnum)) {
        if (!__irq_has_hook_impl(irqnum)) {
            irq_hooks[irqnum] = hookfn;
            pic_set_enabled(irqnum, 1);
            kprintf("irq: irq %d hooked at %p\n", irqnum, (void *) hookfn);
            return 0;
        } else {
            ksnprintf(fail_str, sizeof(fail_str), "already hooked at %p",
                (void *) irq_hooks[irqnum]);
    	}
    } else {
        ksnprintf(fail_str, sizeof(fail_str), "invalid irqnum");
    }

    kprintf("irq: failed to hook irq %d at %p: %s\n", irqnum,
        (void *) hookfn);

    return 1;
}

int irq_remove_hook(int irqnum)
{
    if (__irq_is_valid_irqnum_impl(irqnum)) {
        if (__irq_has_hook_impl(irqnum)) {
            irq_hook_t old_hook = irq_hooks[irqnum];
            irq_hooks[irqnum] = DEFAULT_HOOK_FUNC;
            pic_set_enabled(irqnum, 0);
            kprintf("irq: irq %d unhooked from %p\n", irqnum,
                (void *) old_hook);
            return 0;
        }
    }

    return 1;
}

int irq_call_hook(int irqnum)
{
    if (__irq_is_valid_irqnum_impl(irqnum)) {
        return __irq_call_hook_impl(irqnum);
    }

    return 1;
}

int irq_done(int irqnum)
{
    pic_end_of_interrupt(irqnum);
    return 0;
}
