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

// Dumb waiting function
void dumb_wait(int mult)
{
    static volatile int a;
    volatile int *p = &a;
    for (int i = 0; i < mult * 1000000; ++i) {
        a = *p;
        *p = a;
    }
}          

void CDECL NO_RETURN
kmain(void)
{ 
    if (con_init() || 
        idt_init() || 
        isr_init() ||
        syscall_init() || 
        irq_init() || 
        ps2_init()) {
        panic("init error\n");
    }

    panic_set_use_bsod(1);

    sti();

    // Non-essential drivers (whose initialisation upon which we don't depend)
    kb_init();
    //mouse_init();
    vga_init();

    while (1);
}
