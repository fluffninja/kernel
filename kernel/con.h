#ifndef _INC_CON
#define _INC_CON 1

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

void con_init(void);
void con_clear(void);
void con_write_char(char c);
void con_write_str(const char *str);
void con_set_bgcol(int col);
void con_set_fgcol(int col);

#endif /* _INC_CON */
