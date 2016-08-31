#include <sys/asm.h>
#include <stddef.h>
#include <stdint.h>

#include "ps2.h"
#include "con.h"
#include "pic.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "kstring.h"

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
    con_init();
    pic_init();
    idt_init();
    isr_init();
    irq_init();
    // ps2_init();
    sti();

    while (1);
}
