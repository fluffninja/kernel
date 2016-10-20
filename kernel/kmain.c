#include <stdint.h>
#include <stddef.h>

#include "boot.h"
#include "con.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "kb.h"
#include "kio.h"
#include "panic.h"
#include "mouse.h"
#include "ps2.h"
#include "vga.h"
#include "syscall.h"

void CDECL NO_RETURN
kmain(void)
{
    // Get the boot parameter block left by the bootloader
    struct kernel_boot_params *params = boot_get_params(BOOT_PARAM_BLOCK_ADDR);

    // Initialise the console so that we've got some kind of output
    if (con_init(params)) {
        // If we cannot initialise the console - awkward.
        // There's not a lot we can do about this, so panic.
        panic("init error: console problem\n");
    }

    // Initialise the CPU - that is: the IDT and exception ISRs
    if (idt_init() || isr_init()) {
        panic("init error: cpu problem\n");
    }

    // Initialise hardware things.
    // We initialise the PS2 controller first, so that we can tell
    // it not to spam us with IRQs before we've gotten our IRQ management
    // sorted out.
    if (ps2_init() || irq_init()) {
        panic("init error: hardware interrupt problem\n");
    }

    // Initiliase system call interface
    if (syscall_init()) {
        panic("init error: syscall problem\n");
    }

    // We're ready to accept interrupts now
    sti();

    // From now on, we don't need to panic if a component fails to load.

    // Keyboard
    kb_init();

    // Basic VGA driver - we assume that VGA functions are safely usable
    // after this.
    vga_init();

    // Enable VGA cursor by setting shape
    con_set_cursor_shape(CON_CURSOR_SHAPE_UNDERLINE);

    while (1);
}
