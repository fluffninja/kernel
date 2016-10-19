#include <sys/asm.h>
#include <ctype.h>

#include "kb.h"
#include "kutil.h"
#include "kio.h"
#include "irq.h"
#include "ps2.h"
#include "con.h"
#include "panic.h"

static const int s_scancodes[128] = {
    #include "keymap-en-us"
};

static int s_shift  = 0;
static int s_alt    = 0;
static int s_ctrl   = 0;

static int process_command(char c)
{
    if (c == 'l') {
        // CTRL+L clears the screen
        con_clear();
    } else if (c == 'p') {
        // CTRL+P triggers kernel panic
        panic("triggered panic (ctrl+p)\n");
    } else if (c == 'a') {
        // CTRL+A puts cursor at line start
        int y;
        con_get_cursor_location(NULL, &y);
        con_set_cursor_location(0, y);
    } else {
        return 1;
    }

    return 0;
}

static int kb_irq_hook(int irqnum)
{
    uint8_t raw_data = inportb(PS2_PORT_DATA);
    uint8_t scancode = raw_data & 0x7f;
    int is_key_release = raw_data & 0x80;
    int key_char = s_scancodes[scancode];

    if (key_char & 0x80) {
        switch (key_char) {
        case KB_KEY_LSHIFT:
        case KB_KEY_RSHIFT:
            s_shift = !is_key_release;
            break;
        case KB_KEY_LALT:
        case KB_KEY_RALT:
            s_alt = !is_key_release;
            break;
        case KB_KEY_LCTRL:
        case KB_KEY_RCTRL:
            s_ctrl = !is_key_release;
            break;
        default:
            /* Unknown */
            break;
        }
    } else if (!is_key_release) {
        if (s_ctrl) {
            // If control is held, process keyboard
            // commands (which are CTRL+... combos)
            if (process_command(key_char)) {
                // If no command exists for the
                // given key combo, output a caret
                // and the typed key
                con_write_char('^');
                con_write_char(toupper(key_char));
            }
        } else {
            // If control is not held, type text

            // Convert to caps if shift is held
            if (s_shift && islower(key_char)) {
                key_char = toupper(key_char);
            }

            con_write_char(key_char);
        }
    }

    irq_done(irqnum);
    return 0;
}

int kb_init(void)
{
    if (irq_set_hook(1, kb_irq_hook)) {
        kprintf("kb: failed to hook irq\n");
        return 1;
    }

    kprintf("kb: irq hooked\n");

    ps2_set_enabled(1, 1);

    return 0;
}
