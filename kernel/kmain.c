#include <sys/asm.h>
#include <stddef.h>
#include "ps2.h"

static uint8_t *s_vram_ptr = (uint8_t *) 0xb8000;
static uint32_t s_cursor_pos_x = 0;
static uint32_t s_cursor_pos_y = 0;

void print_char(char c)
{
    if (c == '\n') {
        s_cursor_pos_x = 0;
        ++s_cursor_pos_y;
    } else {
        s_vram_ptr[(s_cursor_pos_x + s_cursor_pos_y * 80) * 2] = c;
        ++s_cursor_pos_x;
    }
}

void print_str(const char *str)
{
    char c;
    while ((c = *(str++))) {
        print_char(c);
    }
}

void print_hex(uint8_t val)
{
    static const char HEX_DIGITS[16] = "0123456789ABCDEF";
    print_char(HEX_DIGITS[val >> 4]);
    print_char(HEX_DIGITS[val & 0x0f]);
}

uint8_t get_scancode(void)
{
    // Wait until the input buffer isn't empty
    if (!(inb(PS2_DATA_PORT) & 0x01)) {
        return 0;
    }
    uint8_t scancode = inb(PS2_DATA_PORT);

    return scancode;
}

void init_conio(void)
{
    uint32_t cursor_pos_x = *((uint8_t *) 0x7e00);
    uint32_t cursor_pos_y = *((uint8_t *) 0x7e01);

    if (cursor_pos_x >= 80 || cursor_pos_y >= 25) {
        cursor_pos_x = 0;
        cursor_pos_y = 0;
    }    

    s_cursor_pos_x = cursor_pos_x;
    s_cursor_pos_y = cursor_pos_y;
}

void CDECL NO_RETURN kmain(void)
{
    init_conio();

    print_str("Welcome to kernel land!\n");

    while (1) {
        uint8_t scancode = get_scancode();
        if (scancode) {
            print_hex(scancode);
        }
    }
}
