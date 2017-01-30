// Useful resources:
// 8084     - http://wiki.osdev.org/%228042%22_PS/2_Controller
// Keyboard - http://wiki.osdev.org/Keyboard_Controller
//          - http://www.computer-engineering.org/ps2keyboard/
// Mouse    - http://wiki.osdev.org/PS/2_Mouse
//          - http://www.computer-engineering.org/ps2mouse/

#ifndef _INC_PS2
#define _INC_PS2 1

#include <kernel/klog.h>
#include <kernel/types.h>
#include <kernel/compiler.h>
#include <kernel/asm/portio.h>

// PS/2 Ports
#define PS2_PORT_DATA                       0x60 // Data Port (Read/Write)
#define PS2_PORT_STATUS                     0x64 // Status Register (Read)
#define PS2_PORT_CMD                        0x64 // Command Register (Write)

// PS/2 Status Flags
#define PS2_STATUS_OUTPUT_BUFFER_FULL       0x01 // Don't out to PS2_PORT_DATA
#define PS2_STATUS_INPUT_BUFFER_FULL        0x02 // Something in PS2_PORT_DATA
#define PS2_STATUS_SYSTEM                   0x04 // System is powered on
#define PS2_STATUS_A2                       0x08 // PS2_PORT_STATUS written to
#define PS2_STATUS_KB_INHIBITED             0x10 // Keyboard is inhibited
#define PS2_STATUS_MOUSE_OUTPUT_BUFFER_FULL 0x20 // Don't out to PS2_PORT_DATA
#define PS2_STATUS_ERROR_TIME_OUT           0x40 // Keyboard didn't respond
#define PS2_STATUS_ERROR_PARITY             0x80 // Communication error

// PS/2 Controller Configuration Flags
// See PS2_CMD_GET_CONFIG and PS2_CMD_SET_CONFIG
#define PS2_CONFIG_CH1_ENABLE_INTERRUPTS    0x01 // Channel 1 invokes interrupt
#define PS2_CONFIG_CH2_ENABLE_INTERRUPTS    0x02 // Channel 2 invokes interrupt
#define PS2_CONFIG_POST_PASSED              0x04 // System POST passed
#define PS2_CONFIG_CH1_CLOCK_HIGH           0x10 // Channel 1 clock is high
#define PS2_CONFIG_CH2_CLOCK_HIGH           0x20 // Channel 2 clock is high
#define PS2_CONFIG_CH1_TRANSLATION          0x40 // Channel 1 translation

// PS/2 Controller Output Flags
// See PS2_CMD_GET_OUTPUT_FLAGS and PS2_CMD_SET_OUTPUT_FLAGS
#define PS2_OUT_SYSTEM_RESET                0x01 // System reset
#define PS2_OUT_A20                         0x02
#define PS2_OUT_CH2_CLOCK                   0x04 // Channel 2 clock high
#define PS2_OUT_CH2_DATA                    0x08 // Channel 2 data high
#define PS2_OUT_CH1_OUTPUT_FULL             0x10 // Channel 1 buffer full
#define PS2_OUT_CH2_OUTPUT_FULL             0x20 // Channel 2 buffer full
#define PS2_OUT_CH1_CLOCK                   0x40 // Channel 1 clock high
#define PS2_OUT_CH1_DATA                    0x80 // Channel 1 data high

// PS/2 Controller Commands
#define PS2_CMD_GET_CONFIG                  0x20 // Get the configuration byte
#define PS2_CMD_SET_CONFIG                  0x60 // Set the configuration byte
#define PS2_CMD_CH2_DISABLE                 0xa7 // Disable channel 2
#define PS2_CMD_CH2_ENABLE                  0xa8 // Enable channel 2
#define PS2_CMD_CH2_TEST                    0xa9 // Test channel 2
#define PS2_CMD_CONTROLLER_TEST             0xaa // Test controller
#define PS2_CMD_CH1_TEST                    0xab // Test channel 1
#define PS2_CMD_DUMP                        0xac // Dump internal RAM
#define PS2_CMD_CH1_DISABLE                 0xad // Disable channel 1
#define PS2_CMD_CH1_ENABLE                  0xae // Enable channel 1
#define PS2_CMD_READ_INPUT_PORT             0xc0 // Read data port
#define PS2_CMD_INPUT_LO_TO_STATUS_HI       0xc1 // Input 0:3 to status 4:7
#define PS2_CMD_INPUT_HI_TO_STATUS_HI       0xc2 // Input 4:7 to status 4:7
#define PS2_CMD_GET_OUTPUT_FLAGS            0xd0 // Get the output flags byte
#define PS2_CMD_SET_OUTPUT_FLAGS            0xd1 // Set the output flags byte
#define PS2_CMD_CH1_WRITE_OUTPUT            0xd2 // Fake data from channel 1
#define PS2_CMD_CH2_WRITE_OUTPUT            0xde // Fake data from channel 2
#define PS2_CMD_CH2_CMD                     0xd4 // Next command goes to ch. 2

// PS/2 Controller Command Responses
#define PS2_RESP_CH_TEST_PASSED             0x00 // Channel self-test passed
#define PS2_RESP_CONTROLLER_TEST_PASSED     0x55 // Controller self-test passed
#define PS2_RESP_OK                         0xfa // Generic success
#define PS2_RESP_FAIL                       0xfc // Generic failure

// PS/2 Common Device Commands
#define PS2_DEV_CMD_IDENTIFY                0xf2 // Request device ID (below)
#define PS2_DEV_CMD_RESEND                  0xfe // Resend last byte
#define PS2_DEV_CMD_TEST                    0xff // Reset and self-test

// PS/2 Common Device Command Responses
#define PS2_DEV_RESP_TEST_PASSED            0xaa // Device self-test passed
#define PS2_DEV_RESP_OK                     0xfa // Generic device success
#define PS2_DEV_RESP_FAIL                   0xfc // Generic device failure
#define PS2_DEV_RESP_RESEND                 0xfe // Generic dev. resend request

// PS/2 Device IDs
// Note also that this list is very incomplete
#define PS2_DEVICE_ID_MOUSE                 0x00
#define PS2_DEVICE_ID_MOUSE_WITH_WHEEL      0x03
#define PS2_DEVICE_ID_MOUSE_5_BUTTON        0x04

// PS/2 Keyboard Commands
#define PS2_KB_CMD_SET_LED                  0xed // Set keyboard LEDs
#define PS2_KB_CMD_ECHO                     0xee
#define PS2_KB_CMD_SET_SCANCODE_SET         0xf0
#define PS2_KB_CMD_IDENTIFY                 0xf2
#define PS2_KB_CMD_SET_TYPEMATIC_CONFIG     0xf3 // Set typematic config byte
#define PS2_KB_CMD_ENABLE_SCANNING          0xf4
#define PS2_KB_CMD_DISABLE_SCANNING         0xf5
#define PS2_KB_CMD_SET_DEFAULTS             0xf6
#define PS2_KB_CMD_SET_ALL_TPMTC            0xf7 // Set all typematic
#define PS2_KB_CMD_SET_ALL_MK_BRK           0xf8 // Set all make/break
#define PS2_KB_CMD_SET_ALL_MK               0xf9 // Set all make
#define PS2_KB_CMD_SET_ALL_TPMTC_MK_BRK     0xfa // Set all typematic/make/brk
#define PS2_KB_CMD_SET_KEY_TPMTC            0xfb // Set key typematic
#define PS2_KB_CMD_SET_KEY_MK_BRK           0xfc // Set key make/break
#define PS2_KB_CMD_SET_KEY_MK               0xfd // Set key make
#define PS2_KB_CMD_RESEND                   0xfe
#define PS2_KB_CMD_RESET                    0xff

// PS/2 Keyboard LED States
#define PS2_KB_LED_SCOLL_LOCK               0
#define PS2_KB_LED_NUM_LOCK                 1
#define PS2_KB_LED_CAPS_LOCK                2

// PS/2 Keyboard Scancode-Set Settings
#define PS2_KB_SCANCODE_SET_CURRENT         0
#define PS2_KB_SCANCODE_SET_1               1
#define PS2_KB_SCANCODE_SET_2               2
#define PS2_KB_SCANCODE_SET_3               3

// PS/2 Keyboard Typematic Byte
BEGIN_PACK struct ps2_kb_typematic_byte {
    u8 repeat_rate     : 5; // 0x00 to 0x1F (30Hz to 2Hz)
    u8 typematic_delay : 2; // Values defined below
} END_PACK;

// PS/2 Keyboard Typematic Byte Repeat Delay Values (in milliseconds)
#define PS2_KB_TYPEMATIC_DELAY_250          0
#define PS2_KB_TYPEMATIC_DELAY_500          1
#define PS2_KB_TYPEMATIC_DELAY_750          2
#define PS2_KB_TYPEMATIC_DELAY_1000         3

// PS/2 Keyboard Command Responses
#define PS2_KB_RESP_ECHO                    0xee // Response to PS2_KB_CMD_ECHO
#define PS2_KB_RESP_RESEND                  0xfe // Device wants resend

// PS/2 Mouse Commands
#define PS2_MOUSE_CMD_SET_SCALING_1_1       0xe6 // Set scaling 1:1
#define PS2_MOUSE_CMD_SET_SCALING_2_1       0xe7 // Set scaling 2:1
#define PS2_MOUSE_CMD_SET_RESOLUTION        0xe8 // Values defined below
#define PS2_MOUSE_CMD_GET_STATUS            0xe9 // Gets a status packet (3B)
#define PS2_MOUSE_CMD_SET_STREAM_MODE       0xea
#define PS2_MOUSE_CMD_READ_DATA             0xeb
#define PS2_MOUSE_CMD_RESET_WRAP_MODE       0xec
#define PS2_MOUSE_CMD_SET_WRAP_MODE         0xee
#define PS2_MOUSE_CMD_SET_REMOTE_MODE       0xf0
#define PS2_MOUSE_CMD_SET_SAMPLE_RATE       0xf3 // Values 10 - 200
#define PS2_MOUSE_CMD_ENABLE_DATA           0xf4 // Enable data transmission
#define PS2_MOUSE_CMD_DISABLE_DATA          0xf5 // Disable data transmission
#define PS2_MOUSE_CMD_SET_DEFAULTS          0xf6

// PS/2 Mouse Status Packet
struct ps2_mouse_status_packet {
    u8 flags;
    u8 resolution;
    u8 sample_rate;
};

// PS/2 Mouse Status Packet Flags
#define PS2_MOUSE_STATUS_LEFT_BTN           0x01
#define PS2_MOUSE_STATUS_RIGHT_BTN          0x02
#define PS2_MOUSE_STATUS_MIDDLE_BTN         0x04
#define PS2_MOUSE_STATUS_SCALING_2_1        0x10 // Scaling is 2:1, not 1:1
#define PS2_MOUSE_STATUS_DATA_ENABLED       0x20
#define PS2_MOUSE_STATUS_IS_REMOTE_MODE     0x40

// PS/2 Mouse Move Packet
struct ps2_mouse_move_packet {
    u8 flags;
    u8 x_movement;
    u8 y_movement;
};

// PS/2 Mouse Move Packet Flags (bit 3 always 1)
#define PS2_MOUSE_MOVE_LEFT_BTN             0x01
#define PS2_MOUSE_MOVE_RIGHT_BTN            0x02
#define PS2_MOUSE_MOVE_MIDDLE_BTN           0x04
#define PS2_MOUSE_MOVE_X_SIGNED             0x10
#define PS2_MOUSE_MOVE_Y_SIGNED             0x20
#define PS2_MOUSE_MOVE_X_OVERFLOW           0x40
#define PS2_MOUSE_MOVE_Y_OVERFLOW           0x80

// PS/2 Mouse Resolution Values
#define PS2_MOUSE_RESOLUTION_1              0
#define PS2_MOUSE_RESOLUTION_2              1
#define PS2_MOUSE_RESOLUTION_4              2
#define PS2_MOUSE_RESOLUTION_8              3

int ps2_init(void);
int ps2_set_enabled(int chnum, int enabled);
u8 ps2_get_config(void);
void ps2_set_config(u8 ps2_config);

#define PS2_OUTPUT_BUFFER_WAIT_TIMEOUT      1024
#define PS2_INPUT_BUFFER_FLUSH_ATTEMPTS     1024

static ALWAYS_INLINE int __flush_input_buffer(void)
{
    int i = PS2_INPUT_BUFFER_FLUSH_ATTEMPTS;
    while (i--) {
        u8 status = inportb(PS2_PORT_STATUS);
        if (status & PS2_STATUS_INPUT_BUFFER_FULL) {
            (void) inportb(PS2_PORT_DATA);
        } else {
            return 0;
        }
        portwait();
    }

    return 1;
}

static ALWAYS_INLINE int __wait_for_output_buffer(void)
{
    int i = PS2_OUTPUT_BUFFER_WAIT_TIMEOUT;
    while (i--) {
        u8 status = inportb(PS2_PORT_STATUS);
        if (!(status & PS2_STATUS_OUTPUT_BUFFER_FULL)) {
            return 0;
        }
        portwait();
    }

    return 1;
}

// Help us find where unnecessary flushes are
#define FLUSH_INPUT_BUFFER()                                                \
    {                                                                       \
        if (__flush_input_buffer()) {                                       \
            klog_printf("ps2: input buffer flush failed (%s:%d)\n",         \
                __FILE__, __LINE__);                                        \
        }                                                                   \
    }

// Help us find where unnecessary waits are
#define WAIT_FOR_OUTPUT_BUFFER()                                            \
    {                                                                       \
        if (__wait_for_output_buffer()) {                                   \
            klog_printf("ps2: output buffer wait failed (%s:%d)\n",         \
                __FILE__, __LINE__);                                        \
        }                                                                   \
    }

#endif /* _INC_PS2 */
