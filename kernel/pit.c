#include "pit.h"

static unsigned long pit_mono_clock_ticks = 0;

typedef struct sleepable_callback_t {
    unsigned long delay_ticks;
    unsigned long last_run_ticks;
    unsigned long (*callback)();
} sleepable_callback_t;

sleepable_callback_t callbacks[16] = {{0,100,0}};
long next_callback_check_in = 1;

unsigned long pit_get_ms()
{
    return (pit_mono_clock_ticks);
}

int pit_tick(int irqnum)
{
    pit_mono_clock_ticks++;
    if(--next_callback_check_in <= 0) {
        for(int cb_i = 0; cb_i < 16; cb_i++) if(callbacks[cb_i].callback) {
            if(callbacks[cb_i].last_run_ticks + callbacks[cb_i].delay_ticks <= pit_mono_clock_ticks) {
                callbacks[cb_i].last_run_ticks = pit_mono_clock_ticks;
                callbacks[cb_i].delay_ticks = callbacks[cb_i].callback();
            }
        }
        int min_delay = 1 << 16;
        for(int cb_i = 0; cb_i < 16; cb_i++) if(callbacks[cb_i].callback) {
            int delay = callbacks[cb_i].last_run_ticks + callbacks[cb_i].delay_ticks - pit_mono_clock_ticks;
            min_delay = min_delay < delay ? min_delay : delay;
        }
        next_callback_check_in = min_delay;
    }
    irq_done(0);
    return 0;
}

static int h = 0;

static unsigned long print_ticks()
{
    int cx, cy;
    con_get_cursor_location(&cx, &cy);
    con_set_cursor_location(60, 0);
    kprintf("Ticks: %d", pit_mono_clock_ticks);
    con_set_cursor_location(cx, cy);
    return 10;
}

static unsigned long print_ticks2()
{
    int cx, cy;
    con_get_cursor_location(&cx, &cy);
    con_set_cursor_location(60, 1);
    kprintf("Ticks: %d", pit_mono_clock_ticks);
    con_set_cursor_location(cx, cy);
    return 7;
}

int pit_init(void)
{
    callbacks[0] = (sleepable_callback_t){.delay_ticks=0, .last_run_ticks=0, .callback=&print_ticks};
    callbacks[1] = (sleepable_callback_t){.delay_ticks=0, .last_run_ticks=0, .callback=&print_ticks2};
    establish_pit(1193);
    irq_set_hook(0, pit_tick);
    kprintf("pit: init\n");
    return 0;
}