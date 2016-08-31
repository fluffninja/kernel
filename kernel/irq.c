#include "irq.h"
#include "isr.h"
#include "pic.h"
#include "kstring.h"

ISR_DEF_HANDLER(isr_irq0);
ISR_DEF_HANDLER(isr_irq1);
ISR_DEF_HANDLER(isr_irq2);
ISR_DEF_HANDLER(isr_irq3);
ISR_DEF_HANDLER(isr_irq4);
ISR_DEF_HANDLER(isr_irq5);
ISR_DEF_HANDLER(isr_irq6);
ISR_DEF_HANDLER(isr_irq7);
ISR_DEF_HANDLER(isr_irq8);
ISR_DEF_HANDLER(isr_irq9);
ISR_DEF_HANDLER(isr_irq10);
ISR_DEF_HANDLER(isr_irq11);
ISR_DEF_HANDLER(isr_irq12);
ISR_DEF_HANDLER(isr_irq13);
ISR_DEF_HANDLER(isr_irq14);
ISR_DEF_HANDLER(isr_irq15);

int irq_init(void)
{
    // PIC Master IRQs
    isr_set_handler(0x20, ISR_HANDLER(isr_irq0));
    isr_set_handler(0x21, ISR_HANDLER(isr_irq1));
    isr_set_handler(0x22, ISR_HANDLER(isr_irq2));
    isr_set_handler(0x23, ISR_HANDLER(isr_irq3));
    isr_set_handler(0x24, ISR_HANDLER(isr_irq4));
    isr_set_handler(0x25, ISR_HANDLER(isr_irq5));
    isr_set_handler(0x26, ISR_HANDLER(isr_irq6));
    isr_set_handler(0x27, ISR_HANDLER(isr_irq7));

    // PIC Slave IRQs
    isr_set_handler(0x28, ISR_HANDLER(isr_irq8));
    isr_set_handler(0x29, ISR_HANDLER(isr_irq9));
    isr_set_handler(0x2a, ISR_HANDLER(isr_irq10));
    isr_set_handler(0x2b, ISR_HANDLER(isr_irq11));
    isr_set_handler(0x2c, ISR_HANDLER(isr_irq12));
    isr_set_handler(0x2d, ISR_HANDLER(isr_irq13));
    isr_set_handler(0x2e, ISR_HANDLER(isr_irq14));
    isr_set_handler(0x2f, ISR_HANDLER(isr_irq15));

    kprintf("irq: loaded handlers\n");

    return 0;
}

void isr_irq0(struct register_set regset)
{
    (void) regset;
    pic_master_end_of_interrupt();
}

void isr_irq1(struct register_set regset)
{
    (void) regset;
    pic_master_end_of_interrupt();    
}

void isr_irq2(struct register_set regset)
{
    // Should never get used, but defined for the sake of completeness.
    // Is used internally by PIC cascade.
    (void) regset;
    kprintf("cpu: irq2\n");
    pic_master_end_of_interrupt();
}

void isr_irq3(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq3\n");
    pic_master_end_of_interrupt();
}

void isr_irq4(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq4\n");
    pic_master_end_of_interrupt();
}

void isr_irq5(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq5\n");
    pic_master_end_of_interrupt();
}

void isr_irq6(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq6\n");
    pic_master_end_of_interrupt();
}

void isr_irq7(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq7\n");
    pic_master_end_of_interrupt();
}

void isr_irq8(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq8\n");
    pic_slave_end_of_interrupt();
}

void isr_irq9(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq9\n");
    pic_slave_end_of_interrupt();
}

void isr_irq10(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq10\n");
    pic_slave_end_of_interrupt();
}

void isr_irq11(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq11\n");
    pic_slave_end_of_interrupt();
}

void isr_irq12(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq12\n");
    pic_slave_end_of_interrupt();
}

void isr_irq13(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq13\n");
    pic_slave_end_of_interrupt();
}

void isr_irq14(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq14\n");
    pic_slave_end_of_interrupt();
}

void isr_irq15(struct register_set regset)
{
    (void) regset;
    kprintf("cpu: irq15\n");
    pic_slave_end_of_interrupt();
}
