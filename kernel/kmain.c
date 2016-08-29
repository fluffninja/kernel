#include <sys/asm.h>
#include <stddef.h>
#include <stdint.h>

#include "ps2.h"
#include "con.h"
#include "kstring.h"

#if 0
void init_ps2(void)
{
    // Disable both devices
    outb(PS2_PORT_CMD, PS2_CMD_CH1_DISABLE);
    outb(PS2_PORT_CMD, PS2_CMD_CH2_DISABLE);

    // Flush input buffer
    while (inb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_WAITING) {
        inb(PS2_PORT_DATA);
    }    

    // Get Config
    outb(PS2_PORT_CMD, PS2_CMD_GET_CONFIG);
    uint8_t ps2_config = inb(PS2_PORT_DATA);

    // Disable all IRQs and translation
    ps2_config &= ~PS2_CONFIG_CH1_ENABLE_INTERRUPTS;
    ps2_config &= ~PS2_CONFIG_CH2_ENABLE_INTERRUPTS;
    ps2_config &= ~PS2_CONFIG_CH1_TRANSLATION;

    // Set Config
    outb(PS2_PORT_CMD, PS2_CMD_SET_CONFIG);
    outb(PS2_PORT_DATA, ps2_config);

    // Perform controller self-test
    outb(PS2_PORT_CMD, PS2_CMD_CONTROLLER_TEST);
    if (inb(PS2_PORT_DATA) == PS2_RESP_CONTROLLER_TEST_PASSED) {
        print_string("PS2 Working\n");
    } else {        
        print_string("PS2 Error\n");
    }

    int is_dual_channel = 0;

    // Potentially dual-channel
    if(ps2_config & PS2_CONFIG_CH2_CLOCK_HIGH) {
        // Re-enable second PS2 port
        outb(PS2_PORT_CMD, PS2_CMD_CH2_ENABLE);

        // Flush input buffer
        while (inb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_WAITING) {
            inb(PS2_PORT_DATA);
        }    

        // Re-read config
        outb(PS2_PORT_CMD, PS2_CMD_GET_CONFIG);
        uint8_t ps2_config = inb(PS2_PORT_DATA);

        if(!(ps2_config & PS2_CONFIG_CH2_CLOCK_HIGH)) {
            is_dual_channel = 1;
            outb(PS2_PORT_CMD, PS2_CMD_CH2_DISABLE);            
        }
    } 

    if (is_dual_channel) {
        print_string("PS2 is dual-channel\n");
    } else {        
        print_string("PS2 is single-channel\n");
    }

    int channel_1_ok = 0;
    int channel_2_ok = 0;

    // Test channel 1
    outb(PS2_PORT_CMD, PS2_CMD_CH1_TEST);
    uint8_t test_code = inb(PS2_PORT_DATA);
    if (test_code == PS2_RESP_CH_TEST_PASSED) {
        print_string("PS2 Channel 1 Working\n");
        channel_1_ok = 1;
    } else {        
        print_string("PS2 Channel 1 Error: ");
        //print_hex8(test_code);
        print_char('\n');
    }

    if (is_dual_channel) {        
        // Test channel 2
        outb(PS2_PORT_CMD, PS2_CMD_CH2_TEST);
        uint8_t test_code = inb(PS2_PORT_DATA);
        if (test_code == PS2_RESP_CH_TEST_PASSED) {
            print_string("PS2 Channel 2 Working\n");
            channel_2_ok = 1;
        } else {        
            print_string("PS2 Channel 2 Error: ");
            //print_hex8(test_code);
            print_char('\n');
        }
    }

    if (!channel_1_ok && !channel_2_ok) {
        print_string("Neither PS2 Channel Working");
        return;
    }

    if (channel_1_ok) {        
        print_string("Using PS2 Channel 1\n");
        outb(PS2_PORT_CMD, PS2_CMD_CH1_ENABLE);
        ps2_config |= PS2_CONFIG_CH1_ENABLE_INTERRUPTS;
    }

    if (channel_2_ok) {
        print_string("Using PS2 Channel 2\n");
        outb(PS2_PORT_CMD, PS2_CMD_CH2_ENABLE);        
        ps2_config |= PS2_CONFIG_CH1_ENABLE_INTERRUPTS;
    }

    // Set Config
    outb(PS2_PORT_CMD, PS2_CMD_SET_CONFIG);
    outb(PS2_PORT_DATA, ps2_config);

    // Perform controller self-test
    outb(PS2_PORT_CMD, PS2_CMD_CONTROLLER_TEST);
    if (inb(PS2_PORT_DATA) == PS2_RESP_CONTROLLER_TEST_PASSED) {
        print_string("PS2 Working\n");
    } else {        
        print_string("PS2 Error\n");
    }

    if (channel_1_ok) {
        // Flush input buffer
        while (inb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_WAITING) {
            inb(PS2_PORT_DATA);
        }            

        outb(PS2_PORT_CMD, PS2_DEV_CMD_TEST);
        uint8_t resp = inb(PS2_PORT_DATA);

        if (resp == PS2_DEV_RESP_TEST_PASSED) {
            print_string("PS2 Channel 1 Device Success\n");            
        } else if (resp == PS2_DEV_RESP_FAIL) {
            print_string("PS2 Channel 1 Device Error\n");                        
        } else {
            print_string("PS2 Channel 1 Device Unknown Response: \n");                        
            //print_hex8(resp);
            print_char('\n');
        }
    }

    if (channel_2_ok) {
        // Write to channel 2
        outb(PS2_PORT_CMD, PS2_CMD_CH2_CMD);

        // Flush input buffer
        while (inb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_WAITING) {
            inb(PS2_PORT_DATA);
        }            

        outb(PS2_PORT_CMD, PS2_DEV_CMD_TEST);
        uint8_t resp = inb(PS2_PORT_DATA);

        if (resp == PS2_DEV_RESP_TEST_PASSED) {
            print_string("PS2 Channel 2 Device Success\n");            
        } else if (resp == PS2_DEV_RESP_FAIL) {
            print_string("PS2 Channel 2 Device Error\n");                        
        } else {
            print_string("PS2 Channel 2 Device Unknown Response: \n");                        
            //print_hex8(resp);
            print_char('\n');
        }   
    }
}
#endif

// Dumb waiting function
void dumbwait(void)
{
    static volatile int a;
    volatile int *p = &a;
    for (int i = 0; i < 500000; ++i) {
        a = *p;
        *p = a;
    }
}

void CDECL NO_RETURN 
kmain(void)
{ 
    con_init();

    while (1);
}
