#include <sys/asm.h>
#include <stddef.h>
#include <stdint.h>

#include "ps2.h"
#include "kio.h"

void ps2_flush_input_data(void)
{
    // TODO: Something like this for PS2_STATUS_OUTPUT_BUSY as well
    while (1) {
        uint8_t status = inportb(PS2_PORT_STATUS);
        if (status & PS2_STATUS_INPUT_WAITING) {
            inportb(PS2_PORT_DATA);
        } else {
            return;
        }
    }
}

void ps2_ch1_enable(void)
{
    kprintf("ps2: enabling channel 1\n");
    outportb(PS2_PORT_CMD, PS2_CMD_CH1_ENABLE);
}

void ps2_ch2_enable(void)
{
    kprintf("ps2: enabling channel 2\n");
    outportb(PS2_PORT_CMD, PS2_CMD_CH2_ENABLE);
}

void ps2_ch1_disable(void)
{
    kprintf("ps2: disabling channel 1\n");
    outportb(PS2_PORT_CMD, PS2_CMD_CH1_DISABLE);
}

void ps2_ch2_disable(void)
{
    kprintf("ps2: disabling channel 2\n");
    outportb(PS2_PORT_CMD, PS2_CMD_CH2_DISABLE);
}

uint8_t ps2_get_config(void)
{    
    ps2_flush_input_data();
    outportb(PS2_PORT_CMD, PS2_CMD_GET_CONFIG);
    uint8_t ps2_config = inportb(PS2_PORT_DATA);    
    kprintf("ps2: config retrieved: %x\n", ps2_config);
    return ps2_config;
}

void ps2_set_config(uint8_t ps2_config)
{
    kprintf("ps2: config set: %x\n", ps2_config);
    outportb(PS2_PORT_CMD, PS2_CMD_SET_CONFIG);
    outportb(PS2_PORT_DATA, ps2_config);
}

static const char *const str_passed = "passed";
static const char *const str_failed = "failed";

int ps2_controller_test(void)
{
    ps2_flush_input_data();
    outportb(PS2_PORT_CMD, PS2_CMD_CONTROLLER_TEST);
    uint8_t status = inportb(PS2_PORT_DATA);
    int passed = (status == PS2_RESP_CONTROLLER_TEST_PASSED);
    kprintf("ps2: controller self-test %s\n", passed ? str_passed : str_failed);
    return passed;
}

int ps2_ch1_test(void)
{
    ps2_flush_input_data();
    outportb(PS2_PORT_CMD, PS2_CMD_CH1_TEST);
    uint8_t status = inportb(PS2_PORT_DATA);
    int passed = (status == PS2_RESP_CH_TEST_PASSED);
    kprintf("ps2: channel 1 self-test %s\n", passed ? str_passed : str_failed);
    return passed;
}

int ps2_ch2_test(void)
{
    ps2_flush_input_data();
    outportb(PS2_PORT_CMD, PS2_CMD_CH2_TEST);
    uint8_t status = inportb(PS2_PORT_DATA);
    int passed = (status == PS2_RESP_CH_TEST_PASSED);
    kprintf("ps2: channel 2 self-test %s\n", passed ? str_passed : str_failed);
    return passed;
}

int ps2_init(void)
{
    int is_dual_channel = 0;
    int ch1_ok = 0;
    int ch2_ok = 0;

    kprintf("ps2: initialising...\n");

    // Disable both devices
    ps2_ch1_disable();
    ps2_ch2_disable();

    // Get Config
    uint32_t ps2_config = ps2_get_config();

    // Disable all IRQs and translation
    ps2_config &= ~PS2_CONFIG_CH1_ENABLE_INTERRUPTS;
    ps2_config &= ~PS2_CONFIG_CH2_ENABLE_INTERRUPTS;
    ps2_config &= ~PS2_CONFIG_CH1_TRANSLATION;

    // Set Config
    ps2_set_config(ps2_config);

    // Perform controller self-test
    if (!ps2_controller_test()) {
        goto error;
    }    

    kprintf("ps2: testing for dual-channel\n");
    // Potentially dual-channel
    if(ps2_config & PS2_CONFIG_CH2_CLOCK_HIGH) {
        // Re-enable second PS2 port
        ps2_ch2_enable();

        // Flush input buffer
        ps2_flush_input_data();

        // Re-read config
        uint8_t ps2_config = ps2_get_config();

        if(!(ps2_config & PS2_CONFIG_CH2_CLOCK_HIGH)) {
            is_dual_channel = 1;
            ps2_ch2_disable();
        }
    }

    kprintf("ps2: dual channel: %b\n", is_dual_channel);

    ch1_ok = ps2_ch1_test();
    if (is_dual_channel) {
        ch2_ok = ps2_ch2_test();
    }

    if (ch1_ok) {        
        ps2_ch1_enable();
        ps2_config |= PS2_CONFIG_CH1_ENABLE_INTERRUPTS;
    }

    if (ch2_ok) {
        ps2_ch2_enable();
        ps2_config |= PS2_CONFIG_CH1_ENABLE_INTERRUPTS;
    }

    // Set Config
    ps2_set_config(ps2_config);

    // Perform controller self-test
    if (!ps2_controller_test()) {
        goto error;
    }    

    ps2_ch1_enable();
    ps2_ch2_enable();

#if 0
    if (ch1_ok) {
        // Flush input buffer
        ps2_flush_input_data();

        outportb(PS2_PORT_CMD, PS2_DEV_CMD_TEST);
        uint8_t resp = inportb(PS2_PORT_DATA);

        if (resp == PS2_DEV_RESP_TEST_PASSED) {
            kprintf("ps2: Channel 1 device success\n");
        } else if (resp == PS2_DEV_RESP_FAIL) {
            kprintf("ps2: Channel 1 device error\n");                        
        } else {
            kprintf("ps2: Channel 1 device unknown response: %x\n", resp);
        }   
    }

    if (ch2_ok) {
        // Write to channel 2
        outportb(PS2_PORT_CMD, PS2_CMD_CH2_CMD);

        // Flush input buffer
        ps2_flush_input_data();

        outportb(PS2_PORT_CMD, PS2_DEV_CMD_TEST);
        uint8_t resp = inportb(PS2_PORT_DATA);

        if (resp == PS2_DEV_RESP_TEST_PASSED) {
            kprintf("ps2: Channel 2 device success\n");            
        } else if (resp == PS2_DEV_RESP_FAIL) {
            kprintf("ps2: Channel 2 device error\n");                        
        } else {
            kprintf("ps2: Channel 2 device unknown response: %x\n", resp);
        }   
    }
#endif

    return 0;

error:
    kprintf("ps2: cancelling initialisation\n");
    return 1;
}
