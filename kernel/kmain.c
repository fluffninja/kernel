#include "con.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "kb.h"
#include "kio.h"
#include "panic.h"
#include "mouse.h"
#include "ps2.h"

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
    panic_set_use_bsod(1);

    if (con_init() || idt_init() || isr_init() || irq_init() || ps2_init()) {
        goto error;
    }

    sti();

    kb_init();
    mouse_init();

    int a = 1;
    int b = 1;
    a--;
    b /= a;
    

    while (1);

error:
    panic("init error\n");
}
