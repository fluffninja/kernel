#include <sys/asm.h>
#include <stddef.h>
#include <stdint.h>

#include "ps2.h"
#include "con.h"
#include "kstring.h"

// Dumb waiting function
void dumb_wait(int mult)
{
    static volatile int a;
    volatile int *p = &a;
    for (int i = 0; i < mult * 1000000; ++i) {
        a = *p;
        *p = a;
    }
}

// PIC - Programmable Interrupt Controller
#define PIC_PORT_MASTER_COMMAND     0x20 // PIC Master Command / Status
#define PIC_PORT_MASTER_DATA        0x21 // PIC Master Data / Mask
#define PIC_PORT_SLAVE_COMMAND      0xa0 // PIC Slave Command / Status
#define PIC_PORT_SLAVE_DATA         0xa1 // PIC Slave Data / Mask

// ICW1 - Initialisation Control Word 1
#define PIC_ICW1_EXPECT_ICW4        0x01 // Expect ICW4 whilst initialising
#define PIC_ICW1_NO_SLAVE           0x02 // Configure to not use slave PIC
#define PIC_ICW1_LEVEL_TRIGGERED    0x08 // Level- rather than edge-triggered
#define PIC_ICW1_INITIALISE         0x10 // Must be set: initialise PIC

// ICW4 - Initialisation Control Word 4 (2 and 3 are values rather than flags)
#define PIC_ICW4_X86_MODE           0x01 // To be used with x86 (must be set)
#define PIC_ICW4_AUTO_EOI           0x02 // Auto EoI on int. acknowledge
#define PIC_ICW4_BUFFER_MASTER      0x04 // Master is buffered (slave must too)
#define PIC_ICW4_BUFFER_SLAVE       0x08 // Slave is buffered 

// OCW2 - Operation Command Word 2 (1 is from just reading the data randomly)
#define PIC_OCW2_EOI                0x20 // End of Interrupt
#define PIC_OCW2_SELECTION          0x40 
#define PIC_OCW2_ROTATE             0x80

// OCW3 - Operation Command Word 3
#define PIC_OCW3_READ_IRR           0x0a // Read Interrupt Request Register
#define PIC_OCW3_READ_ISR           0x0b // Read In-service Register

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
    outportb(PIC_PORT_MASTER_DATA, 0x20);
    portwait();

    // ICW2, interrupt start index, to slave
    outportb(PIC_PORT_SLAVE_DATA, 0x20 + 8);
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
    return 0;
}

static struct idt_entry idt_table[256] = { 0 };

void int_double_fault(void)
{
    ASM_VOLATILE("pusha");
    kprintf("Double-fault\n");
    ASM_VOLATILE("popa; leave; iret");
}

int idt_init(void)
{
    for (size_t i = 0; i < sizeof(idt_table) / sizeof(idt_table[0]); ++i) {
        struct idt_entry df = DEF_IDT_ENTRY(
            &int_double_fault,
            0x08,
            IDT_PRESENT | IDT_PRIVILEGE_0 | IDT_GATE_INTERRUPT_32);
        idt_table[i] = df;
    }

    struct {
        uint16_t s;
        void     *b;
    } PACKED a = { sizeof(idt_table), idt_table };
    //idtd.size = sizeof(idt_table);
    //idtd.base = ((uint32_t)&(idt_table[0])) >> 16;
    //lidt(&idtd);
    ASM_VOLATILE("lidt [eax]"::"a"(&a));
    return 0;
}

void CDECL NO_RETURN 
kmain(void)
{ 
    con_init();

    pic_init();
    kprintf("PIC remapped\n");

    idt_init();
    kprintf("IDT loaded\n");


    kprintf("%p\n", idt_table);
    kprintf("%x\n", sizeof(idt_table));

    sti();

    while (1);
}
