#ifndef _INC_VGA
#define _INC_VGA 1

//
// VGA External Registers
// See: http://www.osdever.net/FreeVGA/vga/extreg.htm
//

// Miscellaneous Output Register Ports
#define VGA_PORT_EXT_MISC_OUT_R                 0x03cc
#define VGA_PORT_EXT_MISC_OUT_W                 0x03c2
#define VGA_PORT_EXT_FEATURE_CTRL_R             0x03ca
#define VGA_PORT_EXT_FEATURE_CTRL_W_MONO        0x03ba
#define VGA_PORT_EXT_FEATURE_CTYL_W_COLOUR      0x03da
#define VGA_PORT_EXT_STATUS_0_R                 0x03c2
#define VGA_PORT_EXT_STATUS_1_R_MONO            0x03ba
#define VGA_PORT_EXT_STATUS_1_R_COLOUR          0x03da

// Flags for VGA_PORT_EXT_MISC_OUT_R
#define VGA_EXT_MISC_COMPATIBLE_CRT_PORTS       0x01    // Change CRT port map*
#define VGA_EXT_MISC_ENABLE_RAM_MAP             0x02    // Map VRAM to RAM
#define VGA_EXT_MISC_CLOCK_25MHZ                0x00    // 25MHz Clock
#define VGA_EXT_MISC_CLOCK_28MHZ                0x04    // 28MHz Clock 
#define VGA_EXT_MISC_SELECT_HIGH_PAGE_ODD_EVEN  0x20    // High page in OE mode
#define VGA_EXT_MISC_HSYNC_POLARITY             0x40    // 0: Positive; 1: Neg.
#define VGA_EXT_MISC_VSYNC_POLARITY             0x80    // 0: Positive; 1: Neg.

// Flags for VGA_PORT_EXT_FEATURE_CTRL_W_MONO/COLOUR
#define VGA_EXT_FEATURE_CTRL_FC1                0x01
#define VGA_EXT_FEATURE_CTRL_FC2                0x02

// Flags for VGA_PORT_EXT_STATUS_0_R
#define VGA_EXT_STATUS_0_SWITCH_SENSE           0x10

// Flags for VGA_PORT_EXT_STATUS_1_R_MONO/COLOUR
#define VGA_EXT_STATUS_1_DISPLAY_DISABLED       0x01
#define VGA_EXT_STATUS_1_VERTICAL_RETRACE       0x08

//
// VGA CRT Registers
// See: http://www.osdever.net/FreeVGA/vga/crtcreg.htm
//

// CRT Control Register Ports
// Note that VGA_EXT_MISC_COMPATIBLE_CRT_PORTS must be set to use there
// Otherwise the following two values need to be 0x03b4 and 0x03b5
#define VGA_PORT_CRT_INDEX_W                    0x03d4
#define VGA_PORT_CRT_DATA_RW                    0x03d5

// CRT Register Indices
#define VGA_CRT_INDEX_HORZ_TOTAL                0x00
#define VGA_CRT_INDEX_END_HORZ_DISPLAY          0x01
#define VGA_CRT_INDEX_BEG_HORZ_BLANK            0x02
#define VGA_CRT_INDEX_END_HORZ_BLANK            0x03
#define VGA_CRT_INDEX_BEG_HORZ_RETRACE          0x04
#define VGA_CRT_INDEX_END_HORZ_RETRACE          0x05
#define VGA_CRT_INDEX_VERT_TOTAL                0x06
#define VGA_CRT_INDEX_OVERFLOW                  0x07
#define VGA_CRT_INDEX_PRESET_ROW_SCAN           0x08
#define VGA_CRT_INDEX_MAX_SCAN                  0x09
#define VGA_CRT_INDEX_CURSOR_START              0x0a
#define VGA_CRT_INDEX_CURSOR_END                0x0b
#define VGA_CRT_INDEX_START_ADDR_HIGH           0x0c
#define VGA_CRT_INDEX_START_ADDR_LOW            0x0d
#define VGA_CRT_INDEX_CURSOR_LOCATION_HIGH      0x0e
#define VGA_CRT_INDEX_CURSOR_LOCATION_LOW       0x0f
#define VGA_CRT_INDEX_BEG_VERT_RETRACE          0x10
#define VGA_CRT_INDEX_END_VERT_RETRACE          0x11
#define VGA_CRT_INDEX_END_VERT_DISPLAY          0x12
#define VGA_CRT_INDEX_OFFSET                    0x13
#define VGA_CRT_INDEX_UNDERLINE                 0x14
#define VGA_CRT_INDEX_BEG_VERT_BLANK            0x15
#define VGA_CRT_INDEX_END_VERT_BLANK            0x16
#define VGA_CRT_INDEX_MODE                      0x17
#define VGA_CRT_INDEX_LINE_COMPARE              0x18

// Flags for VGA_CRT_INDEX_END_HORZ_BLANK
#define VGA_CRT_END_HORZ_BLANK_EVRA             0x80 // END_VERT_RETRACE Access

// Flags for VGA_CRT_INDEX_END_HORZ_RETRACE
#define VGA_CRT_END_HORZ_RETRACE_EHB5           0x80 // END_HORZ_BLANK bit 5

// Flags for VGA_CRT_INDEX_OVERFLOW
#define VGA_CRT_OVERFLOW_VT9                    0x01 // VERT_TOTAL bit 8
#define VGA_CRT_OVERFLOW_EVD8                   0x02 // END_VERT_DISPLAY bit 8
#define VGA_CRT_OVERFLOW_BVR8                   0x04 // BEG_VERT_RETRACE bit 8
#define VGA_CRT_OVERFLOW_BVB8                   0x08 // BEG_VERT_BLANK bit 9
#define VGA_CRT_OVERFLOW_LC8                    0x10 // LINE_COMPARE bit 8
#define VGA_CRT_OVERFLOW_VT9                    0x20 // VERT_TOTAL bit 9
#define VGA_CRT_OVERFLOW_EVD9                   0x40 // END_VERT_DISPLAY bit 9
#define VGA_CRT_OVERFLOW_BVR9                   0x80 // BEG_VERT_RETRACE bit 9

// Flags for VGA_CRT_INDEX_MAX_SCAN
#define VGA_CRT_MAX_SCAN_BVB9                   0x20 // BEG_VERT_BLANK bit 9
#define VGA_CRT_MAX_SCAN_LC9                    0x40 // LINE_COMPARE bit 9
#define VGA_CRT_MAX_SCAN_DOUBLING               0x80

// Flags for VGA_CRT_INDEX_CURSOR_START
#define VGA_CRT_CURSOR_START_DISABLE_CURSOR     0x20

// Flags for VGA_CRT_INDEX_END_VERT_RETRACE
#define VGA_CRT_END_VERT_RETRACE_MEM_BANDWIDTH  0x40
#define VGA_CRT_END_VERT_RETRACE_PROTECT_CRT    0x80

// Flags for VGA_CRT_INDEX_UNDERLINE
#define VGA_CRT_UNDERLINE_DOUBLE_WORD_ADDR      0x40
#define VGA_CRT_UNDERLINE_QUARTER_ADDR_CLOCK    0x20

// Flags for VGA_CRT_INDEX_MODE
#define VGA_CRT_MODE_MAP_DISP_ADDR_13           0x01
#define VGA_CRT_MODE_MAP_DISP_ADDR_14           0x02
#define VGA_CRT_MODE_HALF_SCAN_CLOCK            0x04
#define VGA_CRT_MODE_HALF_ADDR_CLOCK            0x08
#define VGA_CRT_MODE_SELECT_ADDR_WRAP           0x20
#define VGA_CRT_MODE_ENABLE_BYTE_MODE           0x40
#define VGA_CRT_MODE_ENABLE_SYNC                0x80

//
// VGA DAC Registers
// See: http://www.osdever.net/FreeVGA/vga/colorreg.htm
//

// DAC Register Ports
#define VGA_PORT_DAC_STATE_R                    0x03c7
#define VGA_PORT_DAC_INDEX_R                    0x03c7
#define VGA_PORT_DAC_INDEX_W                    0x03c8
#define VGA_PORT_DAC_DATA_RW                    0x03c9

// Flags for VGA_PORT_DAC_STATE_R
#define VGA_DAC_STATE_AWAITING_READ             0x00
#define VGA_DAC_STATE_AWAITING_WRITE            0x03

//
// VGA Graphics Registers
// See: http://www.osdever.net/FreeVGA/vga/graphreg.htm
//

// Graphics Register Ports
#define VGA_PORT_GRAPH_INDEX_W                  0x03ce
#define VGA_PORT_GRAPH_DATA_RW                  0x03cf

// Graphics Register Indices
#define VGA_GRAPH_INDEX_SET_RESET               0x00
#define VGA_GRAPH_INDEX_ENABLE_SET_RESET        0x01
#define VGA_GRAPH_INDEX_COLOUR_COMP             0x02
#define VGA_GRAPH_INDEX_DATA_ROTATE             0x03
#define VGA_GRAPH_INDEX_SELECT_MAP              0x04
#define VGA_GRAPH_INDEX_MODE                    0x05
#define VGA_GRAPH_INDEX_MISC                    0x06
#define VGA_GRAPH_INDEX_COLOUR_COMP_IGNORE      0x07
#define VGA_GRAPH_INDEX_BIT_MASK                0x08

// Flags for VGA_GRAPH_INDEX_DATA_ROTATE
#define VGA_GRAPH_DATA_ROTATE_NONE              0x00
#define VGA_GRAPH_DATA_ROTATE_AND               0x04
#define VGA_GRAPH_DATA_ROTATE_OR                0x08
#define VGA_GRAPH_DATA_ROTATE_XOR               0x0c

// Flags for VGA_GRAPH_INDEX_MODE
#define VGA_GRAPH_MODE_WRITE_MODE_0             0x00
#define VGA_GRAPH_MODE_WRITE_MODE_1             0x01
#define VGA_GRAPH_MODE_WRITE_MODE_2             0x02
#define VGA_GRAPH_MODE_WRITE_MODE_3             0x03
#define VGA_GRAPH_MODE_READ_MODE                0x08
#define VGA_GRAPH_MODE_ODD_EVEN_HOST_READ       0x10
#define VGA_GRAPH_MODE_SHIFT_REG_INTERLEAVE     0x20
#define VGA_GRAPH_MODE_COLOUR_SHIFT_256         0x40

// Flags for VGA_GRAPH_INDEX_MISC
#define VGA_GRAPH_MISC_DISABLE_ALPHANUMERIC     0x01
#define VGA_GRAPH_MISC_CHAIN_ODD_EVEN           0x02
#define VGA_GRAPH_MISC_MEM_MAP_A0000_BFFFF      0x00
#define VGA_GRAPH_MISC_MEM_MAP_A0000_AFFFF      0x04
#define VGA_GRAPH_MISC_MEM_MAP_B0000_B7FFF      0x08
#define VGA_GRAPH_MISC_MEM_MAP_B8000_BFFFF      0x0c

//
// VGA Sequencer Registers
// See: http://www.osdever.net/FreeVGA/vga/seqreg.htm
//

// Sequencer Register Ports
#define VGA_PORT_SEQ_INDEX_R                    0x03c4
#define VGA_PORT_SEQ_DATA_RW                    0x03c5

// Sequencer Register Indices
#define VGA_SEQ_INDEX_RESET                     0x00
#define VGA_SEQ_INDEX_CLOCKING_MODE             0x01
#define VGA_SEQ_INDEX_MAP_MASK                  0x02
#define VGA_SEQ_INDEX_CHARACTER_MAP             0x03
#define VGA_SEQ_INDEX_MEMORY_MODE               0x04

// Flags for VGA_SEQ_RESET
#define VGA_SEQ_RESET_SYNCHRONOUS               0x01
#define VGA_SEQ_RESET_ASYNCHRONOUS              0x02

// Flags for VGA_SEQ_CLOCKING_MODE
#define VGA_SEQ_CLOCKING_MODE_8_DOT_MODE        0x01
#define VGA_SEQ_CLOCKING_MODE_SHIFT_LOAD_RATE   0x04
#define VGA_SEQ_CLOCKING_MODE_DOT_CLOCK_RATE    0x08
#define VGA_SEQ_CLOCKING_MODE_ENABLE_SHIFT_4    0x10
#define VGA_SEQ_CLOCKING_MODE_DISABLE_SCREEN    0x20

//
// VGA Attribute Registers
// http://www.osdever.net/FreeVGA/vga/attrreg.htm
//

// Attribute Register Ports
#define VGA_PORT_ATTR_ADDR_RW                   0x03c0
#define VGA_PORT_ATTR_INDEX_W                   0x03c0
#define VGA_PORT_ATTR_DATA_RW                   0x03c1

// Attribute Register Indices
#define VGA_ATTR_INDEX_MODE                     0x10
#define VGA_ATTR_INDEX_OVERSCAN_COLOR           0x11
#define VGA_ATTR_INDEX_ENABLE_COLOUR_PLANE      0x12
#define VGA_ATTR_INDEX_HORIZONTAL_PIXEL_PAN     0x13
#define VGA_ATTR_INDEX_COLOUR_SELECT            0x14

// Flags for VGA_ATTR_MODE
#define VGA_ATTR_MODE_ENABLE_GRAPHICS           0x01
#define VGA_ATTR_MODE_EMULATE_MONO              0x02
#define VGA_ATTR_MODE_ENABLE_LINE_GRAPHICS      0x04
#define VGA_ATTR_MODE_ENABLE_BLINK              0x08
#define VGA_ATTR_MODE_PIXEL_PANNING             0x02
#define VGA_ATTR_MODE_8_BIT_COLOUR              0x04 // 8-bit / 256 colour
#define VGA_ATTR_MODE_PALETTE_BITS_4_5_SELECT   0x08

int vga_init(void);

#endif /* _INC_VGA */
