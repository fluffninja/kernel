#include <kernel/klog.h>
#include <kernel/types.h>
#include <kernel/asm/portio.h>

#include "mouse.h"
#include "irq.h"
#include "ps2.h"

static int mouse_irq_hook(int irqnum)
{
    while (inportb(PS2_PORT_STATUS) & PS2_STATUS_INPUT_BUFFER_FULL) {
        for (int i = 0; i < 3; ++i) {
            u8 data = inportb(PS2_PORT_DATA);
            (void) data;
        }
    }

    irq_done(irqnum);

    return 0;
}

int mouse_init(void)
{
    if (irq_set_hook(12, mouse_irq_hook)) {
        klog_printf("mouse: failed to hook irq\n");
        return 1;
    }

    klog_printf("mouse: irq hooked\n");

    ps2_set_enabled(2, 1);

    return 0;
}
