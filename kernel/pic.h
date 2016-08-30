#ifndef _INC_PIC
#define _INC_PIC 1

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

int pic_init(void);
void pic_end_of_interrupt(int from_slave);

#endif /* _INC_PIC */
