#include <stdarg.h>

#include <kernel/kernel.h>
#include <kernel/klog.h>
#include <kernel/asm/cpustat.h>
#include <kernel/asm/misc.h>

#include "panic.h"
#include "kio.h"
#include "con.h"

static uint32_t s_panic_flags = PANIC_FULL_DUMP;

#define STACK_WORDS 32
#define STACK_WIDTH 4
#define STACK_ROWS  (STACK_WORDS / STACK_WIDTH)

uint32_t panic_set_flags(uint32_t flags, int state)
{
    if (state) {
        s_panic_flags |= flags;
    } else {
        s_panic_flags &= ~flags;
    }
    return s_panic_flags;
}

static INLINE NO_RETURN void __panic(const struct cpustat cs, const char *fmt,
    va_list args)
{
    char cpustat_dump_buffer[256];

    if (s_panic_flags & PANIC_COLOUR) {
        con_set_background_colour(COL_BLUE);
        con_set_foreground_colour(COL_BRWHITE);
    }

    if (s_panic_flags & PANIC_CLEAR) {
        con_clear();
    }

    klog_printf("panic: ");
    kvprintf(fmt, args);

    if (s_panic_flags & PANIC_HELP_TEXT) {
        kprintf(
            "\n"
            "A problem has been detected and your computer has been\n"
            "frozen to prevent damage to your devices and data.\n"
            "\n"
            "It is OK to turn off and/or restart your computer.\n"
            "\n"
            "Problem details:\n"
        );
    }

    if (s_panic_flags & PANIC_DUMP_CPU) {
        kprintf(" **CPU**\n");
        dump_cpustat(cpustat_dump_buffer, sizeof(cpustat_dump_buffer), cs);
        kprintf("%s", cpustat_dump_buffer);
    }

    if (s_panic_flags & PANIC_DUMP_STACK) {
        kprintf(" **STACK**\n");
        hexdump(
            (int *) ((int) cs.regset.sp & ~(STACK_WIDTH * sizeof(int) - 1)),
            STACK_WIDTH,
            STACK_ROWS);
    }

    cli();
    hlt();
    while (1);
}

void NO_RETURN paniccs(const struct cpustat cs, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    __panic(cs, fmt, args);
    va_end(args);
}

void NO_RETURN panic(const char *fmt, ...)
{
    struct cpustat cs = collect_cpustat();
    va_list args;
    va_start(args, fmt);
    __panic(cs, fmt, args);
    va_end(args);
}
