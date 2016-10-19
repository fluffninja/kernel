#include <ctype.h>
#include <stdint.h>

#include "con.h"
#include "kutil.h"
#include "kio.h"

#define TAB_WIDTH 4

struct charinfo
{
    char            c;
    unsigned char   flags;
};

static uint32_t s_con_flags = 0;

uint32_t con_set_flags(uint32_t flags, int state)
{
    if (state) {
        s_con_flags |= flags;
    } else {
        s_con_flags &= ~flags;
    }
    return s_con_flags;
}

static struct charinfo  *screen_ptr;
static int              s_screen_index;
static int              screen_flags;
static int              s_screen_width;
static int              s_screen_height;

enum
{
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

#if 0
#ifndef ARRSIZE
#define ARRSIZE(A)  (sizeof(A) / sizeof(*A))
#endif

#define VT_COUNT    1

// Virtual-terminal Mode
// Describes VGA-compatible text mode metrics
struct vt_mode
{
    void        *screen_ptr;                    // Flush destination
    int32_t     screen_width;                   // Width
    int32_t     screen_height;                  // Height
};

// List of known modes (immutable)
static const struct vt_mode vt_mode_list[] =
{
    { 0x000b8000, 80, 25, 2 },                  // Text 80x25
};

// Number of known modes
static const int32_t vt_mode_count = (int32_t) ARRSIZE(vt_mode_list);

// Virtual-terminal definition
// Each VT contains a back-buffer location (where data is written to before
// being flushed according to the current mode, and where stored when the Vt
// is inactive), the current write-index withing the buffer (cursor position,
// basically), flags to apply to written items, and the video mode ID to use
// for this VT.
struct vt
{
    int32_t     mode;                           // Video-mode ID
    void        *buffer_ptr;                    // Back-buffer location
    int32_t     buffer_index;                   // Index within back-buffer
    int32_t     screen_index;                   // Index within screen buffer
};

// List of VTs (mutable)
static struct vt vt_list[VT_COUNT];

PACKED struct character
{
    uint8_t blink : 1;
    uint8_t bgcol : 3;
    uint8_t fgcol : 4;
    uint8_t c;
};

static struct
{
    int32_t vt_id;              // Current VT

    void    *buffer_ptr;        // Backbuffer location
    int32_t buffer_index;       // Backbuffer write-index
    size_t  buffer_size;        // Backbuffer size (for alloc/dealloc)

    void    *screen_ptr;        // Flush location
    int32_t screen_index;
    int32_t screen_width;       // Screen width
    int32_t screen_height;      // Screen height
} display;

void init_console(void)
{
    // Attempt to retrieve cursor position
    int cursor_pos_x = (int) *((uint8_t *) 0x7e00);
    int cursor_pos_y = (int) *((uint8_t *) 0x7e01);

    struct vt *vt = &vt_list[0];
    struct vt_mode *mode = &vt_mode_list[vt->mode];

    if (cursor_pos_x >= mode->screen_width ||
        cursor_pos_y >= mode->screen_height) {
        cursor_pos_x = 0;
        cursor_pos_y = 0;
    }
}

#endif
