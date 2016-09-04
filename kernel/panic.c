#include <stdarg.h>
#include <sys/asm.h>

#include "kio.h"
#include "con.h"
#include "panic.h"

static int s_friendly_bsod = 0;

static void print_register_set(struct register_set regset)
{
    kprintf(
        " eax=%x ebx=%x ecx=%x edx=%x\n"
        " esi=%x edi=%x ebp=%x esp=%x\n",
        regset.a, regset.b, regset.c, regset.d,
        regset.si, regset.di, regset.bp, regset.sp);
}

ALWAYS_INLINE NO_RETURN void
__panic(
    const struct register_set   regset,
    const char                  *fmt,
    va_list                     args)
{   
    if (s_friendly_bsod) {
        // Yes, really.
        con_set_bgcol(COL_BLUE);
        con_set_fgcol(COL_BRWHITE);
        con_clear();    
        kprintf(
            "\n"
            " Kernel Panic\n"
            " ============\n"
            "\n"
            "An error has been detected and your computer has been\n"
            "frozen to prevent damage to your devices and data.\n"
            "\n"
            "It is OK to turn off or restart your computer.\n"
            "\n"
            "Details:\n"
            "\n"
        );
    }

    kprintf("panic: ");
    kvprintf(fmt, args);

    print_register_set(regset);

    cli();
    hlt();
    while (1);
}

void NO_RETURN panicrs(const struct register_set regset, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    __panic(regset, fmt, args);
    va_end(args);
}

void NO_RETURN panic(const char *fmt, ...)
{
    struct register_set regset = get_registers();
    va_list args;
    va_start(args, fmt);
    __panic(regset, fmt, args);
    va_end(args);
}
