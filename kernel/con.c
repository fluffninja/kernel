#include <ctype.h>
#include <stdint.h>

#include "con.h"
#include "kutil.h"
#include "kio.h"
#include "vga.h"

#define TAB_WIDTH 4

struct charinfo {
    char            c;
    unsigned char   flags;
};

static struct charinfo  *screen_ptr;
static int              s_screen_index;
static int              screen_flags;
static int              s_screen_width;
static int              s_screen_height;
static uint32_t         s_con_flags = 0;

enum {
    SEEK_REL = 0,
    SEEK_ABS = 1,
};

static int seek_cursor(int offset, int seek)
{
    if (offset && (seek == SEEK_REL)) {
        s_screen_index += offset;
    } else if (seek == SEEK_ABS) {
        s_screen_index = offset;
    }

    vga_set_cursor_location((uint16_t) s_screen_index);

    return s_screen_index;
}

static void scroll_screen(unsigned int lines)
{
    struct charinfo *ptr0;
    struct charinfo *ptr1;
    struct charinfo clearchar;
    unsigned int n;

    ptr0 = screen_ptr;
    ptr1 = ptr0 + s_screen_width * lines;

    // Move the topmost lines upwards
    n = (unsigned int) (s_screen_width * (s_screen_height - lines));
    while (n--) {
        *(ptr0++) = *(ptr1++);
    }

    clearchar.c     = 0;
    clearchar.flags = screen_flags;

    n = (unsigned int) (s_screen_width * lines);
    while (n--) {
        *(ptr0++) = clearchar;
    }

    seek_cursor(-(lines * s_screen_width), SEEK_REL);
}

static void put_char(char c)
{
    struct charinfo info;
    info.c      = c;
    info.flags  = (unsigned char) screen_flags;

    if (s_screen_index >= (s_screen_width * s_screen_height) - 1) {
        scroll_screen(1);
    }

    screen_ptr[s_screen_index] = info;
    seek_cursor(1, SEEK_REL);
}

int con_init(struct kernel_boot_params *params)
{
    int cursor_x = 0;
    int cursor_y = 0;

    screen_ptr = (struct charinfo *) 0xb8000;
    s_screen_width = 80;
    s_screen_height = 25;

    // Use the cursor co-ordinates from the boot parameter block, providing
    // the block is present and the co-ordinates it describes are valid.
    // Note that the cursor co-ordinates in the param block are unsigned bytes.
    if (params &&
        ((int) params->cursor_x < s_screen_width) &&
        ((int) params->cursor_y < s_screen_height)) {

        cursor_x = (int) params->cursor_x;
        cursor_y = (int) params->cursor_y;
    }

    seek_cursor(cursor_x + cursor_y * s_screen_width, SEEK_ABS);

    // Our default flags are the flags of whatever character cell is at the
    // initial cursor position.
    screen_flags = (int) (screen_ptr[s_screen_index].flags);

    kprintf("con: %dx%d at %p\n", s_screen_width, s_screen_height, screen_ptr);

    return 0;
}

uint32_t con_set_flags(uint32_t flags, int state)
{
    if (state) {
        s_con_flags |= flags;
    } else {
        s_con_flags &= ~flags;
    }

    return s_con_flags;
}

void con_clear(void)
{
    scroll_screen(s_screen_height);
    seek_cursor(0, SEEK_ABS);
}

int con_write_char(char c)
{
    if (isprint(c)) {
        put_char(c);
        return 1;
    } else if (c == '\n') {
        int count = s_screen_width - s_screen_index % s_screen_width;
        for (int i = 0; i < count; ++i) {
            put_char(0);
        }
        return 0;
    } else if (c == '\t') {
        int count = TAB_WIDTH - s_screen_index % TAB_WIDTH;
        for (int i = 0; i < count; ++i) {
            put_char(' ');
        }
        return count;
    } else if (c == '\r') {
        seek_cursor(s_screen_index % s_screen_width, SEEK_REL);
        return 0;
    } else if (c == '\b') {
        if (s_screen_index > 0) {
            seek_cursor(-1, SEEK_REL);
            put_char(' ');
            seek_cursor(-1, SEEK_REL);
            return 0;
        }
    } else {
        static const char hex_digits[16] = "0123456789abcdef";
        put_char('~');
        put_char(hex_digits[(c & 0xf0) >> 4]);
        put_char(hex_digits[c & 0x0f]);
        return 3;
    }

    return 0;
}

int con_write_str(const char *str)
{
    if (!str) {
        return 0;
    }

    int char_count = 0;
    while (*str) {
        char_count += con_write_char(*(str++));
    }

    return char_count;
}

void con_set_background_colour(int colour)
{
    int flags = screen_flags;
    flags &= ~(0x07 << 4);
    flags |= ((colour & 0x07) << 4);
    screen_flags = flags;
}

int con_get_background_colour(void)
{
    int flags = screen_flags;
    return ((flags >> 4) & 0x07);
}

void con_set_foreground_colour(int colour)
{
    int flags = screen_flags;
    flags &= ~0x0f;
    flags |= (colour & 0x0f);
    screen_flags = flags;
}

int con_get_foreground_colour(void)
{
    int flags = screen_flags;
    return (flags & 0x0f);
}

int con_set_cursor_location(int x, int y)
{
    if (x < 0 || x >= s_screen_width) {
        return 1;
    }

    if (y < 0 || y >= s_screen_height) {
        return 1;
    }

    seek_cursor(x + y * s_screen_width, SEEK_ABS);

    return 0;
}

void con_get_cursor_location(int *x, int *y)
{
    int xx = s_screen_index % s_screen_width;
    int yy = s_screen_index / s_screen_width;

    if (x) {
        *x = xx;
    }

    if (y) {
        *y = yy;
    }
}

void con_set_cursor_shape(int shape)
{
    switch (shape) {
    default:
    case CON_CURSOR_SHAPE_NONE:
        vga_disable_cursor();
        break;
    case CON_CURSOR_SHAPE_UNDERLINE:
        // TODO: Keep track of the current video mode's character cell size
        // so that we know what size constitutes a 'block' or 'underline'
        // cursor.
        vga_set_cursor_shape(14, 16);
        break;
    case CON_CURSOR_SHAPE_BLOCK:
        vga_set_cursor_shape(0, 16);
        break;
    }
}
