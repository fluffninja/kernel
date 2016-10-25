#include <sys/asm.h>

#include "pic.h"
#include "idt.h"
#include "kio.h"

static int s_master = 0;
static int s_slave = 0;

void pic_get_offsets(int *master, int *slave)
{
    if (master) {
        *master = s_master;
    }

    if (slave) {
        *slave = s_slave;
    }
}

// Check offsets
// Check index validity, and ensure an idt entry is not already present
// -1 denotes success, otherwise returns bad index from offset
static int check_offset_set(int offset)
{
    for (int i = 0; i < 8; ++i) {
        if (!idt_is_valid_index(offset + i) || idt_has_entry(offset + i))
        {
            // Return the index of the offending entry
            return i;
        }
    }

    return -1;
}

// -1 denotes success, otherwise returns bad idt index
static int remap_check_offset_set(int offset)
{
    int index = 0;
    if ((index = check_offset_set(offset)) > 0) {
        kprintf("pic: remap failed, bad index %d on offset %#2x (idt %#2x)\n",
            index, offset, offset + index);
        return (offset + index);
    }

    return -1;
}

int pic_remap(int master, int slave)
{   
    int bad_index = 0;
    if ((bad_index = remap_check_offset_set(slave)) > 0) {
        return bad_index;
    }
    if ((bad_index = remap_check_offset_set(master)) > 0) {
        return bad_index;
    }

    int command;

    // ICW1 - Initialise PIC and say we're sending ICW4 too
    command = PIC_ICW1_INITIALISE | PIC_ICW1_EXPECT_ICW4;
    outportb(PIC_PORT_MASTER_COMMAND, command);
    outportb(PIC_PORT_SLAVE_COMMAND, command);

    // ICW2 - Interrupt offset
    outportb(PIC_PORT_MASTER_DATA, master);
    outportb(PIC_PORT_SLAVE_DATA, slave);

    // ICW3
    outportb(PIC_PORT_MASTER_DATA, 1 << 2); // IRQ (bit) of slave, to master
    outportb(PIC_PORT_MASTER_DATA, 2);      // IRQ (number) of slave, to slave

    // ICW4 - Set x86 mode
    command = PIC_ICW4_X86_MODE;
    outportb(PIC_PORT_MASTER_DATA, command);    
    outportb(PIC_PORT_SLAVE_DATA, command);    

    // Set masks, disabling all interrupts
    outportb(PIC_PORT_MASTER_DATA, 0xff);    
    outportb(PIC_PORT_SLAVE_DATA, 0xff);    

    // That was surprisingly painless :)
    kprintf("pic: remapped master to %#2x, slave to %#2x\n", master, slave);

    return 0;
}

static INLINE void master_set_mask(uint8_t mask)
{
    portwait();
    outportb(PIC_PORT_MASTER_DATA, mask);
}

static INLINE uint8_t master_get_mask(void)
{
    portwait();
    return inportb(PIC_PORT_MASTER_DATA);
}

static INLINE void slave_set_mask(uint8_t mask)
{
    portwait();
    outportb(PIC_PORT_SLAVE_DATA, mask);
}

static INLINE uint8_t slave_get_mask(void)
{
    portwait();
    return inportb(PIC_PORT_SLAVE_DATA);
}

int pic_set_enabled(int irqnum, int enabled)
{
    if (irqnum >= 16) {
        kprintf("pic: invalid irq: %d\n", irqnum);
        return 1;
    }

    uint8_t mask;
    int irqbit = (1 << (irqnum & 7));

    if (irqnum < 8) {
        mask = master_get_mask();
        mask = enabled ? (mask & ~irqbit) : (mask | irqbit);
        master_set_mask(mask);
    } else {
        mask = slave_get_mask();
        mask = enabled ? (mask & ~irqbit) : (mask | irqbit);
        slave_set_mask(mask);
    }

    kprintf("pic: irq %d %s\n", irqnum, enabled ? "enabled" : "disabled");

    return 0;
}

int pic_end_of_interrupt(int irqnum)
{
    if (irqnum >= 8) {
        outportb(PIC_PORT_SLAVE_COMMAND, PIC_OCW2_EOI);
    }

    outportb(PIC_PORT_MASTER_COMMAND, PIC_OCW2_EOI);

    return 0;
}

static uint16_t pic_get_register(int reg)
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
