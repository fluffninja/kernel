#ifndef _INC_PS2
#define _INC_PS2 1

// Useful resources:
// 8084     - http://wiki.osdev.org/%228042%22_PS/2_Controller
// Keyboard - http://wiki.osdev.org/Keyboard_Controller
// Mouse    - http://wiki.osdev.org/PS/2_Mouse
//          - http://www.computer-engineering.org/ps2mouse/

// PS/2 Ports (8084)
#define PS2_DATA_PORT               0x60 // Data Port (Read/Write)
#define PS2_STATUS_PORT             0x64 // Status Register (Read)
#define PS2_CMD_PORT                0x64 // Command Register (Write)

// PS/2 Status Register Flags
#define PS2_STATUS_INPUT_WAITING    0x01 // Data waiting to be read (read it!)
#define PS2_STATUS_OUTPUT_BUSY      0x02 // Data or command busy (don't write!)
#define PS2_STATUS_POST_PASSED      0x04 // POST passed (always set)
#define PS2_STATUS_DATA_IS_CMD      0x08 // Data port contents is a command
#define PS2_STATUS_TIME_OUT_ERROR   0x40
#define PS2_STATUS_PARITY_ERROR     0x80

// PS/2 Controller Configuration Flags
// Read and written via commands (see commands)
#define PS2_CONFIG_INTERRUPT_1      0x01 // PS/2 1 interrupt
#define PS2_CONFIG_INTERRUPT_2      0x02 // PS/2 2 interrupt
#define PS2_CONFIG_POST_PASSED      0x04 // POST passed (always set)
#define PS2_CONFIG_CLOCK_1          0x10 // PS/2 1 clock
#define PS2_CONFIG_CLOCK_2          0x20 // PS/2 2 clock
#define PS2_CONFIG_TRANSLATION_1    0x40 // PS/2 1 translation

// PS/2 Controller Output Port Flags
// Read and written via commands (see commands)
#define PS2_OUT_SYSTEM_RESET        0x01 // System reset 
#define PS2_OUT_A20                 0x02 
#define PS2_OUT_CLOCK_2             0x04 // PS/2 2 clock
#define PS2_OUT_DATA_2              0x08 // PS/2 2 data
#define PS2_OUT_OUTPUT_1_FULL       0x10 // PS/2 1 output buffer full
#define PS2_OUT_OUTPUT_2_FULL       0x20 // PS/2 2 output buffer full
#define PS2_OUT_CLOCK_1             0x40 // PS/2 1 clock
#define PS2_OUT_DATA_1              0x80 // PS/2 1 data

// PS/2 Controller Commands
#define PS2_CMD_GET_CONFIG                  0x20 // Get the configuration byte
#define PS2_CMD_SET_CONFIG                  0x60 // Set the configuration byte
#define PS2_CMD_DISABLE_PS2_2               0xa7 // Disable second PS/2 port
#define PS2_CMD_ENABLE_PS2_2                0xa8 // Enable second PS/2 port
#define PS2_CMD_TEST_PS2_2                  0xa9 // Test second PS/2 port
#define PS2_CMD_TEST_SELF                   0xaa // Test PS/2 controller
#define PS2_CMD_TEST_PS2_1                  0xab // Test first PS/2 port
#define PS2_CMD_DUMP_RAM                    0xac // Dump internal RAM bytes
#define PS2_CMD_DISABLE_PS2_1               0xad // Disable first PS/2 port 
#define PS2_CMD_ENABLE_PS2_1                0xae // Enable first PS/2 port
#define PS2_CMD_READ_INPUT_PORT             0xc0 // Read controller input port
#define PS2_CMD_INPUT_LO_TO_STATUS_HI       0xc1 // Input 0:3 to status 4:7
#define PS2_CMD_INPUT_HI_TO_STATUS_HI       0xc2 // Input 4:7 to status 4:7
#define PS2_CMD_READ_OUTPUT_PORT            0xd0 // Read controller output port
#define PS2_CMD_NEXT_TO_OUTPUT_PORT         0xd1 // Byte to output port
#define PS2_CMD_NEXT_TO_PS2_1_OUTPUT_PORT   0xd2 // Byte to first PS/2 output
#define PS2_CMD_NEXT_TO_PS2_2_OUTPUT_PORT   0xde // Byte to second PS/2 output
#define PS2_CMD_NEXT_TO_PS2_2_INPUT_PORT    0xd4 // Byte to second PS/2 input

// PS/2 Device Commands
#define PS2_DEV_CMD_IDENTIFY        0xf2 // Ask for device ID (see below)

// PS/2 Device Types (response to PS2_DEV_CMD_IDENTIFY)
// Note also that this list is very incomplete
#define PS2_DEVICE_MOUSE            0x00
#define PS2_DEVICE_MOUSE_WITH_WHEEL 0x03
#define PS2_DEVICE_MOUSE_5_BUTTON   0x04

// PS/2 Controller Responses
#define PS2_RESP_TEST_PS2_PASSED    0x00 // PS2_CMD_TEST_PS2_2 or _1 succeeded
#define PS2_RESP_TEST_SELF_PASSED   0x55 // PS2_CMD_TEST_SELF succeeded
#define PS2_RESP_ACK                0xfa // Generic success/acknowledgement
#define PS2_RESP_FAIL               0xfc // Generic failure response

// PS/2 Keyboard Commands
#define PS2_KB_CMD_SET_LED              0xed // Set keyboard LEDs (see below)
#define PS2_KB_CMD_ECHO                 0xee 
#define PS2_KB_CMD_GET_SET_SCANCODE     0xf0 // Set scancode according to set
#define PS2_KB_CMD_SET_TYPEMATIC_CONFIG 0xf3 // Set the typematic config byte
#define PS2_KB_CMD_ENABLE_SCANNING      0xf4
#define PS2_KB_CMD_DISABLE_SCANNING     0xf5
#define PS2_KB_CMD_SET_DEFAULTS         0xf6 
#define PS2_KB_CMD_SET_ALL_TA           0xf7 // Set all typematic/auto-repeat
#define PS2_KB_CMD_SET_ALL_MR           0xf8 // Set all make/release
#define PS2_KB_CMD_SET_ALL_M            0xf9 // Set all make
#define PS2_KB_CMD_SET_ALL_TAMR         0xfa // Set all to all of the above
#define PS2_KB_CMD_SET_KEY_TA           0xfb // Set key typematic/auto-repeat
#define PS2_KB_CMD_SET_KEY_MR           0xfc // Set key make/release
#define PS2_KB_CMD_SET_KEY_M            0xfd // Set key make
#define PS2_KB_CMD_RESEND               0xfe // Resend the last byte
#define PS2_KB_CMD_TEST_SELF            0xff // Reset and self-test

// PS/2 Keyboard LED States (used with PS2_KB_CMD_SET_LED)
#define PS2_KB_LED_SCOLL_LOCK           0
#define PS2_KB_LED_NUM_LOCK             1
#define PS2_KB_LED_CAPS_LOCK            2

// PS/2 Keyboard Scancode-Set Settings (used with PS2_KB_CMD_GET_SET_SCANCODE)
#define PS2_KB_SCANCODE_SET_CURRENT     0
#define PS2_KB_SCANCODE_SET_1           1
#define PS2_KB_SCANCODE_SET_2           2
#define PS2_KB_SCANCODE_SET_3           3

// PS/2 Keyboard Typematic Byte (used with PS2_KB_CMD_SET_TYPEMATIC_CONFIG)
struct ps2_kb_typematic_byte
{
    uint8_t repeat_rate     : 5; // 0x00 = 30Hz; 0x17 = 2Hz
    uint8_t typematic_delay : 2; // Values defined below
};

// PS/2 Keyboard Typematic Byte Repeat Delay Values (in milliseconds)
#define PS2_KB_TYPEMATIC_DELAY_250      0
#define PS2_KB_TYPEMATIC_DELAY_500      1
#define PS2_KB_TYPEMATIC_DELAY_750      2
#define PS2_KB_TYPEMATIC_DELAY_1000     3

// PS/2 Keyboard Responses (to keyboard-specific commands)
#define PS2_KB_RESP_ECHO                0xee // Response to PS2_KB_CMD_ECHO
#define PS2_KB_RESP_TEST_SELF_PASSED    0xaa // PS2_KB_CMD_TEST_SELF succeeded
#define PS2_KB_RESP_RESEND              0xfe // Device wants resend

// PS/2 Mouse Commands
#define PS2_MOUSE_CMD_RESET             0xff
#define PS2_MOUSE_CMD_RESEND            0xfe
#define PS2_MOUSE_CMD_SET_DEFAULTS      0xf6
#define PS2_MOUSE_CMD_DISABLE_DATA      0xf5
#define PS2_MOUSE_CMD_ENABLE_DATA       0xf4
#define PS2_MOUSE_CMD_SET_SAMPLE_RATE   0xf3 // Data is sample rate (10-200)
#define PS2_MOUSE_CMD_SET_REMOTE_MODE   0xf0
#define PS2_MOUSE_CMD_SET_WRAP_MODE     0xee
#define PS2_MOUSE_CMD_RESET_WRAP_MODE   0xec
#define PS2_MOUSE_CMD_READ_DATA         0xeb
#define PS2_MOUSE_CMD_SET_STREAM_MODE   0xea
#define PS2_MOUSE_CMD_GET_STATUS        0xe9 // Mouse will send a status packet
#define PS2_MOUSE_CMD_SET_RESOLUTION    0xe8 // Values defined below
#define PS2_MOUSE_CMD_SET_SCALING_2_1   0xe7 // Set scaling 2:1
#define PS2_MOUSE_CMD_SET_SCALING_1_1   0xe6 // Set scaling 1:1

// PS/2 Mouse Status Packet (from PS2_MOUSE_CMD_GET_STATUS)
struct ps2_mouse_status_packet
{
    uint8_t flags;
    uint8_t resolution;
    uint8_t sample_rate;
};

// PS/2 Mouse Status Packet Flags
#define PS2_MOUSE_STATUS_LEFT_BTN       0x01
#define PS2_MOUSE_STATUS_RIGHT_BTN      0x02
#define PS2_MOUSE_STATUS_MIDDLE_BTN     0x04
#define PS2_MOUSE_STATUS_SCALING        0x10 // Set if scaling is 2:1, not 1:1
#define PS2_MOUSE_STATUS_DATA_ENABLED   0x20 
#define PS2_MOUSE_STATUS_REMOTE_MODE    0x40

// PS/2 Mouse Move Packet
struct ps2_mouse_move_packet
{
    uint8_t flags;
    uint8_t x_movement;
    uint8_t y_movement;
};

// PS/2 Mouse Move Packet Flags (bit 3 always 1)
#define PS2_MOUSE_MOVE_LEFT_BTN         0x01
#define PS2_MOUSE_MOVE_RIGHT_BTN        0x02
#define PS2_MOUSE_MOVE_MIDDLE_BTN       0x04
#define PS2_MOUSE_MOVE_X_SIGNED         0x10
#define PS2_MOUSE_MOVE_Y_SIGNED         0x20
#define PS2_MOUSE_MOVE_X_OVERFLOW       0x40
#define PS2_MOUSE_MOVE_Y_OVERFLOW       0x80

// PS/2 Mouse Resolution Values (used with PS2_MOUSE_CMD_SET_RESOLUTION)
#define PS2_MOUSE_RESOLUTION_1      0
#define PS2_MOUSE_RESOLUTION_2      1
#define PS2_MOUSE_RESOLUTION_4      2
#define PS2_MOUSE_RESOLUTION_8      3

#endif /* _INC_PS2 */
