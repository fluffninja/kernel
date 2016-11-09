#include <stdarg.h>
#include <kernel/asm/cpustat.h>
#include <kernel/asm/misc.h>

#include "kio.h"
#include "con.h"
#include "panic.h"
#include "kutil.h"

static uint32_t s_panic_flags = PANIC_SHOW_DUMP;

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
    if (s_panic_flags & PANIC_USE_COLOUR) {
        con_set_background_colour(COL_BLUE);
        con_set_foreground_colour(COL_BRWHITE);
    }

    if (s_panic_flags & PANIC_CLEAR_SCREEN) {
        con_clear();
    }

    if (s_panic_flags & PANIC_SHOW_HELP) {
        kprintf(
            "\n"
            "A problem has been detected and your computer has been\n"
            "frozen to prevent damage to your devices and data.\n"
            "\n"
            "It is OK to turn off or restart your computer.\n"
            "\n"
            "Problem details:\n"
        );
    }

    kprintf("panic: ");
    kvprintf(fmt, args);

    if (s_panic_flags & PANIC_SHOW_DUMP) {
        kprintf(" **CPU**\n");
        print_cpustat(cs);
        kprintf(" **STACK**\n");
        hexdump((int *) cs.regset.bp, 2, 8);
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
