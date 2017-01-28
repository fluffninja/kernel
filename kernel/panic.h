#ifndef _INC_PANIC
#define _INC_PANIC 1

#include <kernel/compiler.h>
#include <kernel/asm/cpustat.h>

enum {
    PANIC_CLEAR             = 0x0001,
    PANIC_COLOUR            = 0x0002,
    PANIC_HELP_TEXT         = 0x0004,
    PANIC_DUMP_CPU          = 0x0008,
    PANIC_DUMP_STACK        = 0x0010,
    PANIC_FULL_DUMP         = PANIC_DUMP_STACK
                            | PANIC_DUMP_CPU,
    PANIC_BSOD              = PANIC_CLEAR
                            | PANIC_COLOUR
                            | PANIC_HELP_TEXT
                            | PANIC_FULL_DUMP,
};

int panic_set_flags(int flags, int state);
void NO_RETURN paniccs(const struct cpustat cs, const char *fmt, ...);
void NO_RETURN panic(const char *fmt, ...);

#endif /* _INC_PANIC */
