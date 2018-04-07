#include "pit.h"

static unsigned long long pit_mono_clock = 0;

int pit_tick(int irqnum)
{
    kprintf("Tick! %d\n", pit_mono_clock++);
    irq_done(0);
    //cpu_hlt();
    return 0;
}

static int h = 0;

int pit_init(void)
{
    if(h++>0) {
        kprintf("Repeat pit_init :(\n");
        while(1);
    }
    establish_pit(1193 * 10);
    irq_set_hook(0, pit_tick);
    kprintf("pit: init\n");
    return 0;
}