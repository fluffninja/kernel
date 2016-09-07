#include <sys/asm.h>
#include <ctype.h>

#include "kb.h"
#include "kutil.h"
#include "kio.h"
#include "irq.h"
#include "ps2.h"
#include "con.h"

static const int scancodes[128] = {
    #include "keymap-en-us"
};

static int s_b_shift = 0;
static int s_b_alt = 0;
static int s_b_ctrl = 0;

static int kb_irq_hook(int irqnum)
{   
    uint8_t data = inportb(PS2_PORT_DATA);
    uint8_t scancode = data & 0x7f;
    int key_release = data & 0x80;
    int c = scancodes[scancode];

    if (c & 0x80) {
        switch (c) {
        case KB_KEY_LSHIFT:   
        case KB_KEY_RSHIFT:
            s_b_shift = !key_release;
            break;
        case KB_KEY_LALT:   
        case KB_KEY_RALT:
            s_b_alt = !key_release;
            break;
        case KB_KEY_LCTRL:   
        case KB_KEY_RCTRL:
            s_b_ctrl = !key_release;
            break;
        default:
            break;
        }
    } else if (!key_release) {
        if (s_b_ctrl) {
            if (c == 'l') {
                con_clear();
            }
        } else {
            if (s_b_shift && islower(c)) {
                c = toupper(c);
            }
            con_write_char(c); 
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
