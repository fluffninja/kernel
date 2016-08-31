#include <sys/asm.h>

#include "pic.h"
#include "kstring.h"

int pic_init(void)
{
    uint8_t master_offset = 0x20;
    uint8_t slave_offset = 0x28;
    uint8_t command;

    // ICW1 - Initialise PIC and say we're sending ICW4 too
    command = PIC_ICW1_INITIALISE | PIC_ICW1_EXPECT_ICW4;
    outportb(PIC_PORT_MASTER_COMMAND, command);
    outportb(PIC_PORT_SLAVE_COMMAND, command);

    // ICW2 - Interrupt offset
    outportb(PIC_PORT_MASTER_DATA, master_offset);
    outportb(PIC_PORT_SLAVE_DATA, slave_offset);

    // ICW3
    outportb(PIC_PORT_MASTER_DATA, 1 << 2); // IRQ (bit) of slave, to master
    outportb(PIC_PORT_MASTER_DATA, 2);      // IRQ (number) of slave, to slave

    // ICW4 - Set x86 mode
    command = PIC_ICW4_X86_MODE;
    outportb(PIC_PORT_MASTER_DATA, command);    
    outportb(PIC_PORT_SLAVE_DATA, command);    

    // Clear Masks - Allows all IRQs
    outportb(PIC_PORT_MASTER_DATA, 0);    
    outportb(PIC_PORT_SLAVE_DATA, 0);    

    // That was surprisingly painless :)
    kprintf("pic: remapped master at %x, slave at %x\n", master_offset, 
        slave_offset);

    return 0;
}

uint16_t pic_get_register(int reg)
{
    uint16_t master_val;
    uint16_t slave_val;

    portwait();
    outportb(PIC_PORT_MASTER_COMMAND, reg);

    portwait();
    outportb(PIC_PORT_SLAVE_COMMAND, reg);

    portwait();
    master_val = (uint16_t) inportb(PIC_PORT_MASTER_COMMAND);

    portwait();
    slave_val = (uint16_t) inportb(PIC_PORT_SLAVE_COMMAND);

    return (master_val | (slave_val << 8));
}

uint16_t pic_get_irr(void)
{
    return pic_get_register(PIC_OCW3_READ_IRR);
}

uint16_t pic_get_isr(void)
{
    return pic_get_register(PIC_OCW3_READ_ISR);
}

int pic_set_irq_enabled(int irqnum, int enabled)
{
    int master_mask = irqnum & 0xff;
    int slave_mask = irqnum >> 8;
    int mask = 0;

    portwait();
    mask = inportb(PIC_PORT_MASTER_DATA); 
    if (enabled) {
        mask &= ~master_mask;
    } else {
        mask |= master_mask;
    }
    portwait();
    outportb(PIC_PORT_MASTER_DATA, mask);
     
    portwait();
    mask = inportb(PIC_PORT_SLAVE_DATA); 
    if (enabled) {
        mask &= ~slave_mask;
    } else {
        mask |= slave_mask;
    }
    portwait();
    outportb(PIC_PORT_SLAVE_DATA, mask);

    return 0;
}

void pic_master_end_of_interrupt(void)
{
    outportb(PIC_PORT_MASTER_COMMAND, PIC_OCW2_EOI);
}

void pic_slave_end_of_interrupt(void)
{
    outportb(PIC_PORT_SLAVE_COMMAND, PIC_OCW2_EOI);
}
