#include <sys/asm.h>
#include <stdint.h>
#include <cccompat.h>

#include "vga.h"
#include "kio.h"

typedef uint8_t (*adjust_reg_delegate_t)(uint8_t);

ALWAYS_INLINE void __adjust_reg(uint16_t index_port, uint16_t data_port,
    uint8_t regindex, adjust_reg_delegate_t adjustment_delegate)
{
    outportb(index_port, regindex);
    uint8_t value = inportb(data_port);
    value = adjustment_delegate(value);
    outportb(data_port, value);
}

int vga_adjust_reg(uint16_t index_port, uint16_t data_port, uint8_t regindex,
    adjust_reg_delegate_t adjustment_delegate)
{
    if (!adjustment_delegate) {
        return 1;
    }

    __adjust_reg(index_port, data_port, regindex, adjustment_delegate);

    return 0;
}

ALWAYS_INLINE uint8_t __get_reg(uint16_t index_port, uint16_t data_port,
    uint8_t regindex)
{
    outportb(index_port, regindex);
    return inportb(data_port);
}

uint8_t vga_get_reg(uint16_t index_port, uint16_t data_port, uint8_t regindex)
{
    return __get_reg(index_port, data_port, regindex);
}

ALWAYS_INLINE void __set_reg(uint16_t index_port, uint16_t data_port,
    uint8_t regindex, uint8_t value)
{
    outportb(index_port, regindex);
    outportb(data_port, value);
}

void vga_set_reg(uint16_t index_port, uint16_t data_port, uint8_t regindex,
    uint8_t value)
{
    __set_reg(index_port, data_port, regindex, value);
}

void vga_set_reg_list(uint16_t index_port, uint16_t data_port,
    uint8_t start_regindex, uint8_t *values, size_t lsz)
{
    while (lsz--) {
        __set_reg(index_port, data_port, start_regindex++, *(values++));
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

void vga_set_cursor_shape(uint8_t first_line, uint8_t last_line)
{
    first_line &= 0x1f;
    last_line  &= 0x1f;

    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_START, first_line);

    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_END, last_line);
}

void vga_set_cursor_location(uint16_t index)
{
    uint8_t high = (index & 0xff00) >> 8;
    uint8_t low  = (index & 0xff);

    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_LOCATION_HIGH, high);

    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_LOCATION_LOW, low);
}

uint16_t vga_get_cursor_location(void)
{
    int high;
    int low;

    high = (int) __get_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_LOCATION_HIGH);
    low  = (int) __get_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_CURSOR_LOCATION_LOW);

    return (uint16_t) (low | (high << 8));
}

void vga_set_mapping_address(uint32_t address)
{
    int high = (int) (address & 0xff00) >> 8;
    int low  = (int) (address & 0xff);

    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_START_ADDR_HIGH, (uint8_t) high);
    __set_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_START_ADDR_LOW, (uint8_t) low);
}

uint32_t vga_get_mapping_address(void)
{
    int high;
    int low;

    high = (int) __get_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_START_ADDR_HIGH);
    low  = (int) __get_reg(VGA_PORT_CRT_INDEX_W, VGA_PORT_CRT_DATA_RW,
        VGA_CRT_INDEX_START_ADDR_LOW);

    return (uint32_t) (low | (high << 8));
}

void vga_dac_write_rgb(uint8_t dacindex, uint8_t r, uint8_t g, uint8_t b)
{
    outportb(VGA_PORT_DAC_INDEX_W, dacindex);
    outportb(VGA_PORT_DAC_DATA_RW, r);
    outportb(VGA_PORT_DAC_DATA_RW, g);
    outportb(VGA_PORT_DAC_DATA_RW, b);
}

void vga_dac_write_rgb_list(uint8_t start_dacindex, uint8_t *list, size_t lsz)
{
    outportb(VGA_PORT_DAC_INDEX_W, start_dacindex);

    while (lsz--) {
        outportb(VGA_PORT_DAC_DATA_RW, *(list++));
        outportb(VGA_PORT_DAC_DATA_RW, *(list++));
        outportb(VGA_PORT_DAC_DATA_RW, *(list++));
    }
}

int vga_init(void)
{
    uint8_t regval;

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

    kprintf("vga: initialised\n");
    return 1;
}
