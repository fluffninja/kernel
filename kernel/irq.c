#include <sys/asm.h>
#include <stdint.h>
#include <stddef.h>

#include "irq.h"
#include "isr.h"
#include "pic.h"
#include "kstring.h"

static irq_hook_t irq_hooks[16] = { 0 };

#define DEFAULT_HOOK_FUNC   __irq_default_hook_func

static int DEFAULT_HOOK_FUNC(int irqnum)
{
    (void) irqnum;
    return 0;
}

static ALWAYS_INLINE int __irq_is_valid_irqnum_impl(int irqnum)
{
    return !((irqnum < 0) || (irqnum >= (int) ARRLEN(irq_hooks)));
}

static ALWAYS_INLINE int __irq_has_hook_impl(int irqnum) 
{
    return (irq_hooks[irqnum] != DEFAULT_HOOK_FUNC);
}

static ALWAYS_INLINE int __irq_call_hook_impl(int irqnum)
{
    return (irq_hooks[irqnum](irqnum));
}

#define IRQ_ISR_HANDLER(IRQNUM)     ISR_HANDLER(irq_##IRQNUM)

#define IRQ_DEF_ISR_HANDLER(IRQNUM)                 \
    ISR_DEF_HANDLER_BASE(IRQ_ISR_HANDLER(IRQNUM),   \
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
    kprintf("irq: remapping pic\n");

    if (pic_remap(IRQ_PIC_MASTER_IDT_OFFSET, IRQ_PIC_SLAVE_IDT_OFFSET)) {
        kprintf("irq: irq init failed\n");
        return 1;
    }

    for (size_t i = 0; i < 16; ++i) {
        irq_hooks[i] = DEFAULT_HOOK_FUNC;
    }

    // PIC Master IRQs
    isr_set_handler(0x20, IRQ_ISR_HANDLER(0));
    isr_set_handler(0x21, IRQ_ISR_HANDLER(1));
    isr_set_handler(0x22, IRQ_ISR_HANDLER(2));
    isr_set_handler(0x23, IRQ_ISR_HANDLER(3));
    isr_set_handler(0x24, IRQ_ISR_HANDLER(4));
    isr_set_handler(0x25, IRQ_ISR_HANDLER(5));
    isr_set_handler(0x26, IRQ_ISR_HANDLER(6));
    isr_set_handler(0x27, IRQ_ISR_HANDLER(7));

    // PIC Slave IRQs
    isr_set_handler(0x28, IRQ_ISR_HANDLER(8));
    isr_set_handler(0x29, IRQ_ISR_HANDLER(9));
    isr_set_handler(0x2a, IRQ_ISR_HANDLER(10));
    isr_set_handler(0x2b, IRQ_ISR_HANDLER(11));
    isr_set_handler(0x2c, IRQ_ISR_HANDLER(12));
    isr_set_handler(0x2d, IRQ_ISR_HANDLER(13));
    isr_set_handler(0x2e, IRQ_ISR_HANDLER(14));
    isr_set_handler(0x2f, IRQ_ISR_HANDLER(15));

    kprintf("irq: registered pic-irq isr handlers\n");

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
    if (__irq_is_valid_irqnum_impl(irqnum)) {
        if (!__irq_has_hook_impl(irqnum)) {
            irq_hooks[irqnum] = hookfn;
            pic_set_enabled(irqnum, 1);
            kprintf("irq: irq %d hooked at %x\n", irqnum,
                (uint32_t) hookfn);
            return 0;
        }
    }

    return 1;
}

int irq_remove_hook(int irqnum)
{
    if (__irq_is_valid_irqnum_impl(irqnum)) {
        if (__irq_has_hook_impl(irqnum)) {
            irq_hooks[irqnum] = DEFAULT_HOOK_FUNC;
            pic_set_enabled(irqnum, 0);
            kprintf("irq: irq %d unhooked\n", irqnum);
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
