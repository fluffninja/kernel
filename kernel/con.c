#include <ctype.h>
#include <stdint.h>

#include "kstring.h"
#include "con.h"

#define TAB_WIDTH 4

struct charinfo
{
    char            c;
    unsigned char   flags;
};

static struct charinfo  *screen_ptr;
static int              screen_index;
static int              screen_flags;
static int              screen_width;
static int              screen_height;

static void scroll_screen(unsigned int lines)
{
    struct charinfo *ptr0;
    struct charinfo *ptr1;
    struct charinfo clearchar;
    unsigned int n;
    
    ptr0 = screen_ptr;
    ptr1 = ptr0 + screen_width * lines;

    // Move the topmost lines upwards
    n = (unsigned int) (screen_width * (screen_height - lines));
    while (n--) {
        *(ptr0++) = *(ptr1++);
    }

    clearchar.c     = 0;
    clearchar.flags = screen_flags;

    n = (unsigned int) (screen_width * lines);
    while (n--) {
        *(ptr0++) = clearchar;
    }

    screen_index -= lines * screen_width;
}

static void put_char(char c)
{
    struct charinfo info;
    info.c      = c;
    info.flags  = (unsigned char) screen_flags;

    if (screen_index >= (screen_width * screen_height)) {
        scroll_screen(1);
    }

    screen_ptr[screen_index++] = info;
}

int con_init(void)
{
    screen_ptr = (struct charinfo *) 0xb8000;
    screen_width = 80;
    screen_height = 25;

    // Grab the cursor coordinates left by the bootloader
    int cur_x = (int) *((unsigned char *) 0x7e00);
    int cur_y = (int) *((unsigned char *) 0x7e01);   

    // If either coordinate is invalid, just discard both
    if (cur_x >= screen_width || cur_y >= screen_height) {
        cur_x = 0;
        cur_y = 0;
    }

    screen_index = cur_x + cur_y * screen_width;
    screen_flags = (int) (screen_ptr[screen_index].flags);

    kprintf("Console initialised: %dx%d at %p\n", screen_width, screen_height,
        screen_ptr);

    return 0;
}

void con_clear(void)
{
    scroll_screen(screen_height);
    screen_index = 0;
}

int con_write_char(char c)
{
    // If this is a regular print character, 
    if (isprint(c)) {
        put_char(c);
        return 1;
    } else if (c == '\n') {
        int count = screen_width - screen_index % screen_width;
        for (int i = 0; i < count; ++i) {
            put_char(0);            
        }
        return 0;
    } else if (c == '\t') {
        int count = TAB_WIDTH - screen_index % TAB_WIDTH;
        for (int i = 0; i < count; ++i) {
            put_char(' ');            
        }
        return count;
    } else {        
        char buff[3];
        itoa16(c, buff, sizeof(buff) - 1);
        put_char('~');
        con_write_str(buff);
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

void con_set_bgcol(int col)
{
    int flags = screen_flags;
    flags &= ~(0x07 << 4);
    flags |= ((col & 0x07) << 4);
    screen_flags = flags;
}

void con_set_fgcol(int col)
{
    int flags = screen_flags;
    flags &= ~0x0f;
    flags |= (col & 0x0f);
    screen_flags = flags;
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
