#include <kernel/klog.h>
#include <kernel/compiler.h>
#include <kernel/asm/portio.h>

#include "vga.h"

static ALWAYS_INLINE u8 __get_reg(u16 index_port, u16 data_port, u8 index)
{
    outportb(index_port, index);
    return inportb(data_port);
}

static ALWAYS_INLINE void __set_reg(u16 index_port, u16 data_port, u8 index,
    u8 value)
{
    outportb(index_port, index);
    outportb(data_port, value);
}

u8 vga_get_reg(u16 index_port, u16 data_port, u8 index)
{
    return __get_reg(index_port, data_port, index);
}

void vga_set_reg(u16 index_port, u16 data_port, u8 index, u8 value)
{
    __set_reg(index_port, data_port, index, value);
}

void vga_set_reg_list(u16 index_port, u16 data_port, u8 start_index,
    const u8 *value_list, int list_length)
{
    while (list_length--) {
        __set_reg(index_port, data_port, start_index++, *(value_list++));
    }
}

void vga_disable_cursor(void)
{
    // Hide the cursor by setting the disable cursor bit (0x20) of cursor start
    // For good measure, set cursor start to be higher than cursor end

    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_START, 0x21);

    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_END, 0x00);
}

void vga_set_cursor_shape(u8 first_line, u8 last_line)
{
    first_line &= 0x1f;
    last_line  &= 0x1f;

    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_START, first_line);

    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_END, last_line);
}

void vga_set_cursor_location(u16 index)
{
    u8 high = (index & 0xff00) >> 8;
    u8 low  = (index & 0xff);

    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_LOCATION_HIGH, high);

    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_LOCATION_LOW, low);
}

u16 vga_get_cursor_location(void)
{
    int high;
    int low;

    high = (int) __get_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_LOCATION_HIGH);
    low  = (int) __get_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_LOCATION_LOW);

    return (u16) (low | (high << 8));
}

void vga_set_mapping_address(u32 address)
{
    int high = (int) (address & 0xff00) >> 8;
    int low  = (int) (address & 0xff);

    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_START_ADDR_HIGH, (u8) high);
    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_START_ADDR_LOW, (u8) low);
}

u32 vga_get_mapping_address(void)
{
    int high;
    int low;

    high = (int) __get_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_START_ADDR_HIGH);
    low  = (int) __get_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_START_ADDR_LOW);

    return (u32) (low | (high << 8));
}

void vga_dac_write_rgb(u8 index, u8 r, u8 g, u8 b)
{
    outportb(VGA_PORT_DAC_INDEX_W, index);
    outportb(VGA_PORT_DAC_DATA_RW, r);
    outportb(VGA_PORT_DAC_DATA_RW, g);
    outportb(VGA_PORT_DAC_DATA_RW, b);
}

void vga_dac_write_rgb_list(u8 start_index, const u8 *value_list,
    int list_length)
{
    outportb(VGA_PORT_DAC_INDEX_W, start_index);

    while (list_length--) {
        outportb(VGA_PORT_DAC_DATA_RW, *(value_list++));
        outportb(VGA_PORT_DAC_DATA_RW, *(value_list++));
        outportb(VGA_PORT_DAC_DATA_RW, *(value_list++));
    }
}

int vga_init(void)
{
    u8 regval;

    // Turn on compatible port remapping
    regval = inportb(VGA_PORT_EXT_MISC_OUT_R);
    regval |= VGA_EXT_MISC_COMPATIBLE_CRT_PORTS;
    outportb(VGA_PORT_EXT_MISC_OUT_W, regval);

    // Turn off CRT register protection
    // First, enable retrace register access on legacy systems
    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_END_HORZ_BLANK);
    regval = inportb(VGA_PORT_CRT_DATA_RW);
    regval |= VGA_CRT_END_HORZ_BLANK_EVRA;
    outportb(VGA_PORT_CRT_DATA_RW, regval);

    // Turn off protecton bit, in the retrace register
    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_END_VERT_RETRACE);
    regval = inportb(VGA_PORT_CRT_DATA_RW);
    regval &= ~VGA_CRT_END_VERT_RETRACE_PROTECT_CRT;
    outportb(VGA_PORT_CRT_DATA_RW, regval);

    // Perform an inconsequential write to the DAC to normalise its state
    vga_dac_write_rgb(0xff, 0, 0, 0);

    klog_printf("vga: initialised, mapping address at %p\n",
        vga_get_mapping_address());

    return 1;
}
