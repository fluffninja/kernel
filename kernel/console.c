#include <ctype.h>

#include <kernel/kernel.h>
#include <kernel/klog.h>
#include <kernel/kerror.h>
#include <kernel/types.h>
#include <kernel/compiler.h>

#include "console.h"
#include "vga.h"

#define TAB_WIDTH 4

struct video_cell {
    char cellchar;
    u8   flags;
};

static struct video_cell *s_video_ptr;
static int s_index;
static int s_video_cell_flags;
static int s_video_width;
static int s_video_height;

static int set_index(int index)
{
    index = MAX(index, 0);
    index = MIN(index, s_video_width * s_video_height);
    s_index = index;

    // Place the VGA cursor at the location
    vga_set_cursor_location((u16) index);

    return s_index;
}

static void scroll_screen(int lines)
{
    int i;
    struct video_cell *curr_line_ptr;
    struct video_cell *next_line_ptr;
    const struct video_cell clear_cell = {
        .cellchar = 0,
        .flags = (u8) s_video_cell_flags
    };

    lines = MAX(lines, 0);
    lines = MIN(lines, s_video_height);

    curr_line_ptr = s_video_ptr;
    next_line_ptr = curr_line_ptr + s_video_width * lines;

    i = s_video_width * (s_video_height - lines);

    // Move the topmost lines upwards
    while (i--) {
        *(curr_line_ptr++) = *(next_line_ptr++);
    }

    // Clear the remainder of the sreen
    i = s_video_width * lines;

    while (i--) {
        *(curr_line_ptr++) = clear_cell;
    }

    set_index(s_index - lines * s_video_width);
}

static int seek(int offset)
{
    int new_index = set_index(s_index + offset);

    if (new_index >= s_video_width * s_video_height) {
        scroll_screen(
            (new_index - s_video_width * s_video_height) / s_video_height + 1);
    }

    return s_index;
}

static void put_char(char c)
{
    struct video_cell info = {
        .cellchar = c,
        .flags = (u8) s_video_cell_flags
    };

    seek(1);

    s_video_ptr[s_index - 1] = info;
}

int con_init(struct kernel_boot_params *params)
{
    int cursor_x = 0;
    int cursor_y = 0;

    s_video_ptr = (struct video_cell *) 0xb8000;
    s_video_width = 80;
    s_video_height = 25;

    // Use the cursor co-ordinates from the boot parameter block, providing
    // the block is present and the co-ordinates it describes are valid.
    // Note that the cursor co-ordinates in the param block are unsigned bytes.
    if (params &&
        ((int) params->cursor_x < s_video_width) &&
        ((int) params->cursor_y < s_video_height)) {

        cursor_x = (int) params->cursor_x;
        cursor_y = (int) params->cursor_y;
    }

    set_index(cursor_x + cursor_y * s_video_width);

    // Our default flags are the flags of whatever character cell is at the
    // initial cursor position.
    s_video_cell_flags = (int) (s_video_ptr[s_index].flags);

    klog_printf("con: %dx%d at %p\n", s_video_width, s_video_height,
        s_video_ptr);

    return 0;
}

void con_clear(void)
{
    scroll_screen(s_video_height);
    set_index(0);
}

static void type_carriage_return()
{
    seek(-(s_index % s_video_width));
}

static void type_newline()
{
    seek(s_video_width - (s_index % s_video_width));
}

static void type_tabulator()
{
    seek(TAB_WIDTH - s_index % TAB_WIDTH);
}

static void type_backspace()
{
    if (s_index > 0) {
        seek(-1);
        put_char(' ');
        seek(-1);
    }
}

static void type_unknown(char c)
{
    static const char HEX_DIGITS[16] = "0123456789abcdef";

    put_char('~');
    put_char(HEX_DIGITS[(c & 0xf0) >> 4]);
    put_char(HEX_DIGITS[c & 0x0f]);
}

int con_write_char(char c)
{
    if (isprint(c)) {
        put_char(c);
        return 1;
    }

    switch (c) {
    default:
        type_unknown(c);
        break;
    case '\r':
        type_carriage_return();
        break;
    case '\n':
        type_newline();
        break;
    case '\t':
        type_tabulator();
        break;
    case '\b':
        type_backspace();
        break;
    }

    return 0;
}

int con_write_str(const char *str)
{
    if (!str) {
        return KERROR_ARG_NULL;
    }

    while (*str) {
        con_write_char(*(str++));
    }

    return 0;
}

int con_get_background_colour(void)
{
    int flags = s_video_cell_flags;
    return ((flags >> 4) & 0x07);
}

void con_set_background_colour(int colour)
{
    int flags = s_video_cell_flags;
    flags &= ~(0x07 << 4);
    flags |= ((colour & 0x07) << 4);
    s_video_cell_flags = flags;
}

int con_get_foreground_colour(void)
{
    int flags = s_video_cell_flags;
    return (flags & 0x0f);
}

void con_set_foreground_colour(int colour)
{
    int flags = s_video_cell_flags;
    flags &= ~0x0f;
    flags |= (colour & 0x0f);
    s_video_cell_flags = flags;
}

void con_get_cursor_location(int *x, int *y)
{
    int xx = s_index % s_video_width;
    int yy = s_index / s_video_width;

    if (x) {
        *x = xx;
    }

    if (y) {
        *y = yy;
    }
}

int con_set_cursor_location(int x, int y)
{
    if (x < 0 || x >= s_video_width) {
        return KERROR_ARG_OUT_OF_RANGE;
    }

    if (y < 0 || y >= s_video_height) {
        return KERROR_ARG_OUT_OF_RANGE;
    }

    set_index(x + y * s_video_width);

    return 0;
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
