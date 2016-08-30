#include <sys/asm.h>
#include <stddef.h>
#include <stdint.h>

#include "ps2.h"
#include "con.h"
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
    ps2_init();

    for (int i = 0; i < 100; ++i) {
        dumb_wait(25);
        con_set_fgcol(i);
        kprintf("Hello, world!\n");
    }

    while (1);
}
