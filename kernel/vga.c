#include <sys/asm.h>
#include <stdint.h>
#include <cccompat.h>

#include "vga.h"
#include "kio.h"

void vga_hide_cursor(void)
{
    // Hide the cursor by setting the disable cursor bit (0x20) of cursor start
    // For good measure, set cursor start to be higher than cursor end

    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_CURSOR_START);
    outportb(VGA_PORT_CRT_DATA_RW, (uint8_t) 0x21);

    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_CURSOR_END);
    outportb(VGA_PORT_CRT_DATA_RW, (uint8_t) 0x00);    
}

void vga_set_cursor_shape(int start, int end)
{
    start &= 0x1f;
    end   &= 0x1f;

    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_CURSOR_START);
    outportb(VGA_PORT_CRT_DATA_RW, (uint8_t) start);

    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_CURSOR_END);
    outportb(VGA_PORT_CRT_DATA_RW, (uint8_t) end);
}

void vga_set_cursor_location(int index) 
{
    int high = (index & 0xff00) >> 8;
    int low  = (index & 0xff);

    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_CURSOR_LOCATION_HIGH);
    outportb(VGA_PORT_CRT_DATA_RW, (uint8_t) high);

    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_CURSOR_LOCATION_LOW);
    outportb(VGA_PORT_CRT_DATA_RW, (uint8_t) low);    
}

int vga_get_cursor_location(void)
{
    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_CURSOR_LOCATION_HIGH);
    int high = inportb(VGA_PORT_CRT_DATA_RW);

    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_CURSOR_LOCATION_LOW);
    int low = inportb(VGA_PORT_CRT_DATA_RW);     

    return (low | (high << 8));
}

void vga_set_start_address(int address)
{
    int high = (address & 0xff00) >> 8;
    int low  = (address & 0xff);

    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_START_ADDR_HIGH);
    outportb(VGA_PORT_CRT_DATA_RW, (uint8_t) high);

    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_START_ADDR_LOW);
    outportb(VGA_PORT_CRT_DATA_RW, (uint8_t) low);   
}

int vga_get_start_address(void)
{
    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_START_ADDR_HIGH);
    int high = inportb(VGA_PORT_CRT_DATA_RW);

    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_START_ADDR_LOW);
    int low = inportb(VGA_PORT_CRT_DATA_RW);     

    return (low | (high << 8));
}

int vga_dac_write_rgb(uint8_t dac_index, uint8_t r, uint8_t g, uint8_t b)
{
    outportb(VGA_PORT_DAC_INDEX_W, dac_index);
    outportb(VGA_PORT_DAC_DATA_RW, r);
    outportb(VGA_PORT_DAC_DATA_RW, g);
    outportb(VGA_PORT_DAC_DATA_RW, b);    
    return 0;
}

int vga_dac_write_rgb_list(uint8_t dac_index, uint8_t *list, size_t n)
{
    outportb(VGA_PORT_DAC_INDEX_W, dac_index);
    while (n--) {
        outportb(VGA_PORT_DAC_DATA_RW, *(list++));        
        outportb(VGA_PORT_DAC_DATA_RW, *(list++));
        outportb(VGA_PORT_DAC_DATA_RW, *(list++));   
    }
    return 0;
}

int vga_init(void)
{
    // Turn on compatible port remapping
    uint8_t misc_out = inportb(VGA_PORT_EXT_MISC_OUT_R);
    misc_out |= VGA_EXT_MISC_COMPATIBLE_CRT_PORTS;
    outportb(VGA_PORT_EXT_MISC_OUT_W, misc_out);

    // Turn off CRT register protection
    // First, enable retrace register access on legacy systems
    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_END_HORIZONTAL_BLANKING);
    outportb(VGA_PORT_CRT_DATA_RW, inportb(VGA_PORT_CRT_DATA_RW) | 0x80);
    // Turn off protecton bit, in the retrace register
    outportb(VGA_PORT_CRT_INDEX_W, VGA_CRT_INDEX_END_VERTICAL_RETRACE);
    outportb(VGA_PORT_CRT_DATA_RW, inportb(VGA_PORT_CRT_DATA_RW) & ~0x80);

    // Perform an inconsequential write to the DAC to normalise its state
    vga_dac_write_rgb(0xff, 0, 0, 0);


    kprintf("vga: initialised\n");
    return 1;
}
