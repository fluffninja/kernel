

static unsigned char *s_vram_ptr = (unsigned char *) 0xb8000;
static unsigned char s_cursor_pos_x = 0;
static unsigned char s_cursor_pos_y = 0;


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


void init_conio(void)
{
    unsigned char cursor_pos_x = *((unsigned char *) 0x7e00);
    unsigned char cursor_pos_y = *((unsigned char *) 0x7e01);

    if (cursor_pos_x >= 80 || cursor_pos_y >= 25) {
        cursor_pos_x = 0;
        cursor_pos_y = 0;
    }    

    s_cursor_pos_x = cursor_pos_x;
    s_cursor_pos_y = cursor_pos_y;
}


void kmain(void)
{
    init_conio();

    print_str("Welcome to kernel land!");

    while (1);
}
