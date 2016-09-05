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
    uint8_t data = inportb(PS2_PORT_DATA);
    if (!(data & 0x80) && scancodes[data]) {
        con_write_char(scancodes[data]); 
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
