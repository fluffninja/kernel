#include <sys/asm.h>
#include <stddef.h>
#include <stdint.h>

#include "ps2.h"

#define COL_BLACK               0x00
#define COL_BLUE                0x01
#define COL_GREEN               0x02
#define COL_CYAN                0x03
#define COL_RED                 0x04
#define COL_MAGENTA             0x05
#define COL_YELLOW              0x06
#define COL_WHITE               0x07
#define COL_BRBLACK             0x08
#define COL_BRBLUE              0x09
#define COL_BRGREEN             0x0a
#define COL_BRCYAN              0x0b
#define COL_BRRED               0x0c
#define COL_BRMAGENTA           0x0d
#define COL_BRYELLOW            0x0e
#define COL_BRWHITE             0x0f

#define BOOT_CURSOR_X_POS_ADDR  0x7e00
#define BOOT_CURSOR_Y_POS_ADDR  0x7e01
#define TAB_WIDTH               4

static uint8_t *s_vram = (uint8_t *) 0xb8000;

#define SCREEN_WIDTH    80
#define SCREEN_HEIGHT   25
#define SCREEN_SCAN     (SCREEN_WIDTH * 2)
#define SCREEN_LENGTH   (SCREEN_SCAN * SCREEN_HEIGHT)

//static uint8_t ALIGN(4) s_bb[SCREEN_LENGTH];
static uint8_t *s_bb =  (uint8_t *) 0xb8000;
static uint32_t         s_write_index;

void vt_flush(const uint8_t *buffer)
{
    uint32_t *dw_buffer = (uint32_t *) buffer;
    uint32_t *dw_vram   = (uint32_t *) s_vram;
    size_t n = SCREEN_LENGTH;
    while (n--) {
        *(dw_vram++) = *(dw_buffer++);
    }
}

void conio_init(void)
{
    uint32_t cursor_pos_x = (uint32_t) *((uint8_t *) BOOT_CURSOR_X_POS_ADDR);
    uint32_t cursor_pos_y = (uint32_t) *((uint8_t *) BOOT_CURSOR_Y_POS_ADDR);

    if (cursor_pos_x >= SCREEN_WIDTH ||
        cursor_pos_y >= SCREEN_HEIGHT)
    {
        cursor_pos_x = 0;
        cursor_pos_y = 0;
    }

    s_write_index = cursor_pos_x + cursor_pos_y * SCREEN_WIDTH;

    uint32_t *dwvram = (uint32_t *) 0xb8000;
    uint32_t *dwbb   = (uint32_t *) s_bb;
    for (size_t i = 0; i < (80 * 25 * 2) / 4; ++i) {
        *(dwbb++) = *(dwvram++);
    }
}

void print_char(char c)
{
    switch (c) {
    case '\n':
        s_write_index += SCREEN_WIDTH - s_write_index % SCREEN_WIDTH;
        break;
    case '\t':
        s_write_index += TAB_WIDTH - s_write_index % TAB_WIDTH;
        break;
    default:
        s_bb[s_write_index * 2] = c;
        s_bb[s_write_index * 2 + 1] = 0x07;
        ++s_write_index;
    }
    s_write_index %= SCREEN_WIDTH * SCREEN_HEIGHT;
}

void print_string(const char *str)
{
    char c;
    while ((c = *(str++))) {
        print_char(c);
    }
}

void print_hex8(uint8_t val)
{
    static char HEX_DIGITS[16] = "0123456789ABCDEF";
    print_char(HEX_DIGITS[val >> 4]);
    print_char(HEX_DIGITS[val & 0x0f]);
}

void init_ps2(void)
{
    // Disable both devices
    outb(PS2_PORT_CMD, PS2_CMD_DISABLE_PS2_CH1);
    outb(PS2_PORT_CMD, PS2_CMD_DISABLE_PS2_CH2);

    // Flush input buffer
    while (inb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_WAITING) {
        inb(PS2_PORT_DATA);
    }    

    // Get Config
    outb(PS2_PORT_CMD, PS2_CMD_GET_CONFIG);
    uint8_t ps2_config = inb(PS2_PORT_DATA);

    // Disable all IRQs and translation
    ps2_config &= ~PS2_CONFIG_INTERRUPT_CH1;
    ps2_config &= ~PS2_CONFIG_INTERRUPT_CH2;
    ps2_config &= ~PS2_CONFIG_TRANSLATION_CH1;

    // Set Config
    outb(PS2_PORT_CMD, PS2_CMD_SET_CONFIG);
    outb(PS2_PORT_DATA, ps2_config);

    // Perform controller self-test
    outb(PS2_PORT_CMD, PS2_CMD_TEST_SELF);
    if (inb(PS2_PORT_DATA) == PS2_RESP_TEST_SELF_PASSED) {
        print_string("PS2 Working\n");
    } else {        
        print_string("PS2 Error\n");
    }

    int is_dual_channel = 0;

    // Potentially dual-channel
    if(ps2_config & PS2_CONFIG_CLOCK_CH2) {
        // Re-enable second PS2 port
        outb(PS2_PORT_CMD, PS2_CMD_ENABLE_PS2_CH2);

        // Re-read config
        outb(PS2_PORT_CMD, PS2_CMD_GET_CONFIG);
        uint8_t ps2_config = inb(PS2_PORT_DATA);

        if(!(ps2_config & PS2_CONFIG_CLOCK_CH2)) {
            is_dual_channel = 1;
            outb(PS2_PORT_CMD, PS2_CMD_DISABLE_PS2_CH2);            
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
    outb(PS2_PORT_CMD, PS2_CMD_TEST_PS2_CH1);
    uint8_t test_code = inb(PS2_PORT_DATA);
    if (test_code == PS2_RESP_TEST_CH_PASSED) {
        print_string("PS2 Channel 1 Working\n");
        channel_1_ok = 1;
    } else {        
        print_string("PS2 Channel 1 Error: ");
        print_hex8(test_code);
        print_char('\n');
    }

    if (is_dual_channel) {        
        // Test channel 2
        outb(PS2_PORT_CMD, PS2_CMD_TEST_PS2_CH2);
        uint8_t test_code = inb(PS2_PORT_DATA);
        if (test_code == PS2_RESP_TEST_CH_PASSED) {
            print_string("PS2 Channel 2 Working\n");
            channel_2_ok = 1;
        } else {        
            print_string("PS2 Channel 2 Error: ");
            print_hex8(test_code);
            print_char('\n');
        }
    }

    if (!channel_1_ok && !channel_2_ok) {
        print_string("Neither PS2 Channel Working");
        return;
    }

    if (channel_1_ok) {        
        print_string("Using PS2 Channel 1\n");
        outb(PS2_PORT_CMD, PS2_CMD_ENABLE_PS2_CH1);
        ps2_config |= PS2_CONFIG_INTERRUPT_CH1;
    }

    if (channel_2_ok) {
        print_string("Using PS2 Channel 2\n");
        outb(PS2_PORT_CMD, PS2_CMD_ENABLE_PS2_CH2);        
        ps2_config |= PS2_CONFIG_INTERRUPT_CH2;
    }

    // Set Config
    outb(PS2_PORT_CMD, PS2_CMD_SET_CONFIG);
    outb(PS2_PORT_DATA, ps2_config);

    // Perform controller self-test
    outb(PS2_PORT_CMD, PS2_CMD_TEST_SELF);
    if (inb(PS2_PORT_DATA) == PS2_RESP_TEST_SELF_PASSED) {
        print_string("PS2 Working\n");
    } else {        
        print_string("PS2 Error\n");
    }


    if (channel_1_ok) {
        // Flush input buffer
        while (inb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_WAITING) {
            inb(PS2_PORT_DATA);
        }            

        outb(PS2_PORT_CMD, PS2_DEV_CMD_TEST_SELF);
        uint8_t resp = inb(PS2_PORT_DATA);

        if (resp == PS2_RESP_TEST_PASSED) {
            print_string("PS2 Channel 1 Device Success\n");            
        } else if (resp == PS2_RESP_FAIL) {
            print_string("PS2 Channel 1 Device Error\n");                        
        } else {
            print_string("PS2 Channel 1 Device Unknown Response: \n");                        
            print_hex8(resp);
            print_char('\n');
        }
    }

    if (channel_2_ok) {
        // Write to channel 2
        outb(PS2_PORT_CMD, PS2_CMD_NEXT_TO_PS2_CH2_INPUT_PORT);

        // Flush input buffer
        while (inb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_WAITING) {
            inb(PS2_PORT_DATA);
        }            

        outb(PS2_PORT_CMD, PS2_DEV_CMD_TEST_SELF);
        uint8_t resp = inb(PS2_PORT_DATA);

        if (resp == PS2_RESP_TEST_PASSED) {
            print_string("PS2 Channel 2 Device Success\n");            
        } else if (resp == PS2_RESP_FAIL) {
            print_string("PS2 Channel 2 Device Error\n");                        
        } else {
            print_string("PS2 Channel 2 Device Unknown Response: \n");                        
            print_hex8(resp);
            print_char('\n');
        }   
    }
}

void CDECL NO_RETURN 
kmain(void)
{ 
    conio_init();
    init_ps2();

    print_string("\nHello, world!\n");

    while (1);
}
