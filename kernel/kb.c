#include <sys/asm.h>

#include "kb.h"
#include "kio.h"
#include "irq.h"
#include "ps2.h"
#include "con.h"

static char scancodes[128] = {
    #include "keymap-en-us"
};

static int kb_irq_hook(int irqnum)
{
    while (inportb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_WAITING) {
        uint8_t data = inportb(PS2_PORT_DATA);
        if (!(data & 0x80) && scancodes[data]) {
            con_write_char(scancodes[data]); 
        } else {
            /*
            char buff[3] = { 0 };
            extern char *itoa16(int, char*, int);
            itoa16(data, buff, 2);
            kprintf("~%s", buff);
            */
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

    return 0;
}
