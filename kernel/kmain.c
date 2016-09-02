#include <sys/asm.h>
#include <stddef.h>
#include <stdint.h>

#include "kstring.h"
#include "con.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "kb.h"

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

// Reference for IRQs' respective devices:
// https://en.wikipedia.org/wiki/Interrupt_request_(PC_architecture)

void CDECL NO_RETURN 
kmain(void)
{ 
    con_init();
    idt_init();
    isr_init();
    irq_init();
    sti();

    kb_init();

    while (1);
}
