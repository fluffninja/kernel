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
    // Initialise primary systems
    if (con_init() || 
        idt_init() || 
        isr_init() ||
        syscall_init() || 
        ps2_init() || 
        irq_init()) {
        // If any of these fail, we cannot continue
        panic("init error\n");
    }

    // Turn on fullscreen panic
    panic_set_use_bsod(1);

    // Enable interrupts
    sti();

    kb_init();
    vga_init();

    while (1);
}
