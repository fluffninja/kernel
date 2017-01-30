#include <ctype.h>

#include <kernel/kernel.h>
#include <kernel/klog.h>
#include <kernel/asm/misc.h>

#include "boot.h"
#include "con.h"
#include "panic.h"
#include "cpu/idt.h"
#include "cpu/isr.h"
#include "ps2.h"
#include "irq.h"
#include "kb.h"
#include "vga.h"

static int on_key_event(const struct kb_key *key)
{
    static bool b_shift_pressed = false;
    static bool b_ctrl_pressed = false;

    u8 keycode = key->keycode;
    bool is_pressed = key->is_pressed;

    if (keycode & 0x80) {
        switch (keycode) {
        case KB_KEY_LSHIFT:
        case KB_KEY_RSHIFT:
            b_shift_pressed = is_pressed;
            break;
        case KB_KEY_LCTRL:
        case KB_KEY_RCTRL:
            b_ctrl_pressed = is_pressed;
            break;
        }
    } else if (is_pressed) {
        if (b_ctrl_pressed) {
            if (keycode == 'l') {
                con_clear();
            } else if (keycode == 'p') {
                panic("manual panic (ctrl+p)\n");
            } else if (keycode == 'a') {
                int y;
                con_get_cursor_location(NULL, &y);
                con_set_cursor_location(0, y);
            } else {
                con_write_char('^');
                con_write_char(toupper(keycode));
            }
        } else {
            if (b_shift_pressed && islower(keycode)) {
                keycode = toupper(keycode);
            }

            con_write_char(keycode);
        }

    }

    return 0;
}

void CDECL NO_RETURN kmain(void)
{
    // Get the kernel boot parameter block left by the bootloader.
    struct kernel_boot_params *params = boot_get_params(BOOT_PARAM_BLOCK_ADDR);

    // Initialise the console so that we've got some kind of output capability.
    if (con_init(params)) {
        panic("init error: console problem\n");
    }

    // Initialise the CPU: the IDT and CPU exception ISRs.
    if (idt_init() || isr_init()) {
        panic("init error: cpu problem\n");
    }

    // Initialise hardware things.
    // We initialise the PS2 controller first, so that we can tell it not to
    // spam us with IRQs before we've gotten our IRQ management sorted out.
    if (ps2_init() || irq_init()) {
        panic("init error: hardware interrupt problem\n");
    }

    // We're ready to accept interrupts now.
    sti();

    // From now on, we don't need to panic if a component fails to load.

    // Keyboard driver.
    kb_init();
    kb_add_listener(on_key_event);

    // VGA - we assume that VGA functions are safely usable after this.
    vga_init();

    // Enable VGA cursor by setting shape.
    con_set_cursor_shape(CON_CURSOR_SHAPE_UNDERLINE);

    klog_printf("init ok\n");

    while (1);
}
