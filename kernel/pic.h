#ifndef _INC_PIC
#define _INC_PIC 1

#include <stdint.h>

// PIC - Programmable Interrupt Controller
#define PIC_PORT_MASTER_COMMAND     0x20 // PIC Master Command / Status
#define PIC_PORT_MASTER_DATA        0x21 // PIC Master Data / Mask
#define PIC_PORT_SLAVE_COMMAND      0xa0 // PIC Slave Command / Status
#define PIC_PORT_SLAVE_DATA         0xa1 // PIC Slave Data / Mask

// ICW1 - Initialisation Control Word 1
// Send straight to the command port. Note that the presence of the initialise
// flag makes these OCW1 rather than OCW4 (which does not include initialise).
#define PIC_ICW1_EXPECT_ICW4        0x01 // Expect ICW4 whilst initialising
#define PIC_ICW1_SINGLE             0x02 // Only one PIC in system
#define PIC_ICW1_ADDR_INTERVAL_4    0x04 // Interrupt handlers have 4B spacing
#define PIC_ICW1_LEVEL_TRIGGERED    0x08 // Level- rather than edge-triggered
#define PIC_ICW1_INITIALISE         0x10 // Must be set: initialise PIC

// ICW4 - Initialisation Control Word 4 
// Send after ICW1 if the 'expect ICW4' flag is set in ICW1.
#define PIC_ICW4_X86_MODE           0x01 // To be used with x86 (must be set)
#define PIC_ICW4_AUTO_EOI           0x02 // Auto EoI on int. acknowledge
#define PIC_ICW4_BUFFER_MASTER      0x04 // Master is buffered (slave must too)
#define PIC_ICW4_BUFFER_SLAVE       0x08 // Slave is buffered 
#define PIC_ICW4_FULLY_NESTED       0x10 // Many cascading controllers (unused)

// OCW1 - Operation Control Word 1
// OCW1 is whatever data is read or written to the data port without preamble,
// which is interpreted as getting or setting the IRQ masks respectively.

// OCW2 - Operation Command Word 2
// Send after reading from the data port.
#define PIC_OCW2_L1                 0x01 // Interrupt level 1
#define PIC_OCW2_L2                 0x02 // Interrupt level 2
#define PIC_OCW2_L3                 0x04 // Interrupt level 3
#define PIC_OCW2_EOI                0x20 // End of Interrupt
#define PIC_OCW2_SELECT             0x40 
#define PIC_OCW2_ROTATE             0x80

// OCW3 - Operation Command Word 3
// Send straight to the command port. Note that the absence of the initialise
// flag makes these OCW3 rather than OCW1 (which includes initialise).
#define PIC_OCW3_READ_IRR           0x0a // Read Interrupt Request Register
#define PIC_OCW3_READ_ISR           0x0b // Read In-service Register

// Send to both data ports to disable PIC functionality
#define PIC_DISABLE                 0xff

int pic_remap(int master, int slave);
int pic_set_enabled(int irqnum, int enabled);
int pic_end_of_interrupt(int irqnum);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);
void pic_get_offsets(int *master, int *slave);

#endif /* _INC_PIC */
