#include <kernel/asm/portio.h>
#include <stddef.h>
#include <stdint.h>

#include "ps2.h"
#include "kio.h"

uint8_t ps2_get_config(void)
{
    WAIT_FOR_OUTPUT_BUFFER();
    outportb(PS2_PORT_CMD, PS2_CMD_GET_CONFIG);
    uint8_t ps2_config = inportb(PS2_PORT_DATA);
    return ps2_config;
}

void ps2_set_config(uint8_t ps2_config)
{
    WAIT_FOR_OUTPUT_BUFFER();
    outportb(PS2_PORT_CMD, PS2_CMD_SET_CONFIG);
    outportb(PS2_PORT_DATA, ps2_config);
}

int ps2_set_enabled(int chnum, int enabled)
{
    if (chnum != 1 && chnum != 2) {
        return 1;
    }

    uint32_t ps2_config = ps2_get_config();

    if (chnum == 1) {
        if (enabled) {
            WAIT_FOR_OUTPUT_BUFFER();
            outportb(PS2_PORT_CMD, PS2_CMD_CH1_ENABLE);
            ps2_config |= PS2_CONFIG_CH1_ENABLE_INTERRUPTS;
        } else {
            WAIT_FOR_OUTPUT_BUFFER();
            outportb(PS2_PORT_CMD, PS2_CMD_CH1_DISABLE);
            ps2_config &= ~PS2_CONFIG_CH1_ENABLE_INTERRUPTS;
        }
    }
    if (chnum == 2) {
        if (enabled) {
            WAIT_FOR_OUTPUT_BUFFER();
            outportb(PS2_PORT_CMD, PS2_CMD_CH2_ENABLE);
            ps2_config |= PS2_CONFIG_CH2_ENABLE_INTERRUPTS;
        } else {
            WAIT_FOR_OUTPUT_BUFFER();
            outportb(PS2_PORT_CMD, PS2_CMD_CH2_DISABLE);
            ps2_config &= ~PS2_CONFIG_CH2_ENABLE_INTERRUPTS;
        }
    }

    ps2_set_config(ps2_config);

    kprintf("ps2: channel %d %s\n", chnum, enabled ? "enabled" : "disabled");

    return 0;
}

int ps2_disable_scancode_translation(void)
{
    uint32_t ps2_config = ps2_get_config();
    ps2_config &= ~PS2_CONFIG_CH1_TRANSLATION;
    ps2_set_config(ps2_config);

    kprintf("ps2: scancode translation disabled\n");

    return 0;
}

int ps2_init(void)
{
    ps2_set_enabled(1, 0);
    ps2_set_enabled(2, 0);

    return 0;
}
