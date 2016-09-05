#ifndef _INC_VGA
#define _INC_VGA 1

// External Registers
// See: http://www.osdever.net/FreeVGA/vga/extreg.htm

// Miscellaneous Output Register
#define VGA_PORT_EXT_MISC_OUT_R                 0x03cc
#define VGA_PORT_EXT_MISC_OUT_W                 0x03c2

// Feature Control Register
#define VGA_PORT_EXT_FEATURE_CTRL_R             0x03ca
#define VGA_PORT_EXT_FEATURE_CTRL_W_MONO        0x03ba
#define VGA_PORT_EXT_FEATURE_CTYL_W_COLOUR      0x03da

// Input Status #0 Register
#define VGA_PORT_EXT_STATUS_0_R                 0x03c2

// Input Status #1 Register
#define VGA_PORT_EXT_STATUS_1_R_MONO            0x03ba
#define VGA_PORT_EXT_STATUS_1_R_COLOUR          0x03da

// Flags for Miscellaneous Output Register
#define VGA_EXT_MISC_COMPATIBLE_CRT_PORTS       0x01    // Change CRT port map*
#define VGA_EXT_MISC_ENABLE_RAM_MAP             0x02    // Map VRAM to RAM
#define VGA_EXT_MISC_CLOCK_25MHZ                0x00    // 25MHz Clock
#define VGA_EXT_MISC_CLOCK_28MHZ                0x04    // 28MHz Clock 
#define VGA_EXT_MISC_SELECT_HIGH_PAGE           0x20    // High page in OE mode
#define VGA_EXT_MISC_HSYNC_POLARITY             0x40    // 0: Positive; 1: Neg.
#define VGA_EXT_MISC_VSYNC_POLARITY             0x80    // 0: Positive; 1: Neg.

// Flags for Feature Control Register
#define VGA_EXT_FEATURE_CTRL_FC1                0x01
#define VGA_EXT_FEATURE_CTRL_FC2                0x02

// Flags for Input Status #0 Register
#define VGA_EXT_STATUS_0_SWITCH_SENSE           0x10

// Flags for Input Status #1 Register
#define VGA_EXT_STATUS_1_DISPLAY_DISABLED       0x01
#define VGA_EXT_STATUS_1_VERTICAL_RETRACE       0x08

// CRT Control Registers
// Note that VGA_EXT_MISC_COMPATIBLE_CRT_PORTS must be set to use there
// Otherwise the following two values need to be 0x03b4 and 0x03b5
#define VGA_PORT_CRT_INDEX_W                    0x03d4
#define VGA_PORT_CRT_DATA_RW                    0x03d5

// CRT Register Indices
#define VGA_CRT_INDEX_HORIZONTAL_TOTAL          0x00
#define VGA_CRT_INDEX_END_HORIZONTAL_DISPLAY    0x01
#define VGA_CRT_INDEX_START_HORIZONTAL_BLANKING 0x02
#define VGA_CRT_INDEX_END_HORIZONTAL_BLANKING   0x03
#define VGA_CRT_INDEX_START_HORIZONTAL_RETRACE  0x04
#define VGA_CRT_INDEX_END_HORIZONTAL_RETRACE    0x05
#define VGA_CRT_INDEX_VERTICAL_TOTAL            0x06
#define VGA_CRT_INDEX_OVERFLOW                  0x07
#define VGA_CRT_INDEX_PRESET_ROW_SCAN           0x08
#define VGA_CRT_INDEX_MAXIMUM_SCAN_LINE         0x09
#define VGA_CRT_INDEX_CURSOR_START              0x0a
#define VGA_CRT_INDEX_CURSOR_END                0x0b
#define VGA_CRT_INDEX_START_ADDR_HIGH           0x0c
#define VGA_CRT_INDEX_START_ADDR_LOW            0x0d
#define VGA_CRT_INDEX_CURSOR_LOCATION_HIGH      0x0e
#define VGA_CRT_INDEX_CURSOR_LOCATION_LOW       0x0f
#define VGA_CRT_INDEX_START_VERTICAL_RETRACE    0x10
#define VGA_CRT_INDEX_END_VERTICAL_RETRACE      0x11
#define VGA_CRT_INDEX_END_VERTICAL_DISPLAY      0x12
#define VGA_CRT_INDEX_OFFSET                    0x13
#define VGA_CRT_INDEX_UNDERLINE_LOCATION        0x14
#define VGA_CRT_INDEX_START_VERTICAL_BLANKING   0x15
#define VGA_CRT_INDEX_END_VERTICAL_BLANKING     0x16
#define VGA_CRT_INDEX_MODE_CONTROL              0x17
#define VGA_CRT_INDEX_LINE_COMPARE              0x18

// DAC Registers
#define VGA_PORT_DAC_STATE_R                    0x03c7
#define VGA_PORT_DAC_INDEX_R                    0x03c7
#define VGA_PORT_DAC_INDEX_W                    0x03c8
#define VGA_PORT_DAC_DATA_RW                    0x03c9

// Flags for DAC State Register
#define VGA_DAC_STATE_AWAITING_READ             0x00
#define VGA_DAC_STATE_AWAITING_WRITE            0x11

// Graphics Registers
#define VGA_PORT_GRAPHICS_INDEX_W               0x03ce
#define VGA_PORT_GRAPHICS_DATA_RW               0x03cf

// Graphics Register Indices
#define VGA_GRAPHICS_INDEX_SET_RESET            0x00
#define VGA_GRAPHICS_INDEX_ENABLE_SET_RESET     0x01
#define VGA_GRAPHICS_INDEX_COLOUR_COMP          0x02
#define VGA_GRAPHICS_INDEX_DATA_ROTATE          0x03
#define VGA_GRAPHICS_INDEX_MAP_SELECT           0x04
#define VGA_GRAPHICS_INDEX_MODE                 0x05
#define VGA_GRAPHICS_INDEX_MISC                 0x06
#define VGA_GRAPHICS_INDEX_COLOUR_COMP_IGNORE   0x07
#define VGA_GRAPHICS_INDEX_BIT_MASK             0x08

int vga_init(void);

#endif /* _INC_VGA */
