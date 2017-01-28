#include <stddef.h>

#include <kernel/asm/cpustat.h>

#include "../kio.h"

int dump_cpustat(char *buffer, size_t buffer_size, const struct cpustat cs)
{
    return ksnprintf(
        buffer,
        buffer_size,
        " eax=%08x ebx=%08x ecx=%08x edx=%08x\n"
        " esi=%08x edi=%08x ebp=%08x esp=%08x\n"
        " efl=%08x\n",
        cs.regset.a, cs.regset.b, cs.regset.c, cs.regset.d,
        cs.regset.si, cs.regset.di, cs.regset.bp, cs.regset.sp,
        cs.eflags
    );
}
