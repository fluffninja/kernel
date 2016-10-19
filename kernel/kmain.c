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

void call_test_fn(void);

void CDECL NO_RETURN
kmain(void)
{
    struct kernel_boot_params *params = boot_get_params(BOOT_PARAM_BLOCK_ADDR);

    // Initialise primary systems
    if (con_init(params) ||
        idt_init() ||
        isr_init() ||
        syscall_init() ||
        ps2_init() ||
        irq_init()) {
        // If any of these fail, we cannot continue
        panic("init error\n");
    }

    // Enable interrupts
    sti();

    kb_init();
    vga_init();

    while (1);
}
