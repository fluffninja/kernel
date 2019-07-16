#ifndef _INC_CONSOLE
#define _INC_CONSOLE 1

// For struct kernel_boot_params
#include "boot.h"

#define COL_BLACK               0x00
#define COL_BLUE                0x01
#define COL_GREEN               0x02
#define COL_CYAN                0x03
#define COL_RED                 0x04
#define COL_MAGENTA             0x05
#define COL_YELLOW              0x06
#define COL_WHITE               0x07
#define COL_BRBLACK             0x08
#define COL_BRBLUE              0x09
#define COL_BRGREEN             0x0a
#define COL_BRCYAN              0x0b
#define COL_BRRED               0x0c
#define COL_BRMAGENTA           0x0d
#define COL_BRYELLOW            0x0e
#define COL_BRWHITE             0x0f

enum {
    CON_CURSOR_SHAPE_NONE,
    CON_CURSOR_SHAPE_UNDERLINE,
    CON_CURSOR_SHAPE_BLOCK,
};

int con_init(struct kernel_boot_params *params);

void con_clear(void);

int con_write_char(char c);
int con_write_str(const char *str);

int con_get_background_colour(void);
void con_set_background_colour(int col);

int con_get_foreground_colour(void);
void con_set_foreground_colour(int col);

void con_get_cursor_location(int *x, int *y);
int con_set_cursor_location(int x, int y);

void con_set_cursor_shape(int shape);

#endif /* _INC_CONSOLE */
