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
    u8 keycode = key->keycode;

    // Only process key-presses
    if (key->event == KB_PRESS) {
        // If the control key is being held, perform an action
        if (key->modifiers & KB_MOD_CTRL) {
            if (keycode == 'l') {
                // Clear the screen
                con_clear();
            } else if (keycode == 'p') {
                // Manually panic
                panic("manual panic (ctrl+p)\n");
            } else if (keycode == 'a') {
                con_write_char('\r');
            } else {
                // No appropriate command, print the letter preceded by a '^'
                con_write_char('^');
                con_write_char(toupper(keycode));
            }

            return 0;
        }

        // Make sure that this isn't the key-press of a modifier key
        if (key->keycode < 250) {
            // If shift is held, capitalise the character
            if ((key->modifiers & KB_MOD_SHIFT) && islower(keycode)) {
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
