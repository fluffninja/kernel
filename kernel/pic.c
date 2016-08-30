#include <sys/asm.h>

#include "pic.h"
#include "kstring.h"

void pic_end_of_interrupt(int from_slave)
{
    // If the IRQ came from the slave PIC, both master and slave need EoI cmd.
    if (from_slave) {
        outportb(PIC_PORT_SLAVE_COMMAND, PIC_OCW2_EOI);
        portwait();    
    }

    outportb(PIC_PORT_MASTER_COMMAND, PIC_OCW2_EOI);
    portwait();    
}

int pic_init(void)
{
    uint8_t master_offset = 0x20;
    uint8_t slave_offset = 0x28;

    // Don't want to screw this up, so will be paranoid with the portwait()s

    // ICW1 to master
    outportb(PIC_PORT_MASTER_COMMAND,
             PIC_ICW1_INITIALISE | PIC_ICW1_EXPECT_ICW4);
    portwait();    

    // ICW1 to slave
    outportb(PIC_PORT_SLAVE_COMMAND,
             PIC_ICW1_INITIALISE | PIC_ICW1_EXPECT_ICW4);
    portwait();    

    // ICW2, interrupt start index, to master
    outportb(PIC_PORT_MASTER_DATA, master_offset);
    portwait();

    // ICW2, interrupt start index, to slave
    outportb(PIC_PORT_SLAVE_DATA, slave_offset);
    portwait();

    // ICW3, IRQ of slave (2), to master
    outportb(PIC_PORT_MASTER_DATA, 0x04); // 0x04 is Bit #2 (counts from #0)
    portwait();

    // ICW3, IRQ line for cascade to master, to slave
    outportb(PIC_PORT_MASTER_DATA, 2);
    portwait();

    // ICW4, set x86 mode, to master
    outportb(PIC_PORT_MASTER_DATA, PIC_ICW4_X86_MODE);    
    portwait();

    // ICW4, set x86 mode, to slave
    outportb(PIC_PORT_SLAVE_DATA, PIC_ICW4_X86_MODE);    
    portwait();

    // Clear masks on master
    outportb(PIC_PORT_MASTER_DATA, 0);    
    portwait();

    // Clear masks on slave
    outportb(PIC_PORT_SLAVE_DATA, 0);    
    portwait();    

    // That was surprisingly painless :)
    kprintf("PIC remapped: master at %x, slave at %x\n", master_offset, 
        slave_offset);

    return 0;
}
