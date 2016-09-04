#include "con.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "kb.h"
#include "kio.h"
#include "panic.h"

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
    if (con_init() || idt_init() || isr_init() || irq_init()) {
        goto error;
    }

    sti();

    kb_init();

    ASM("int 0xff");

    while (1);

error:
    panic("init error\n");
}
