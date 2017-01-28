#ifndef _INC_KERNEL_ASM_CPUSTAT
#define _INC_KERNEL_ASM_CPUSTAT 1

#include <kernel/kernel.h>
#include <kernel/types.h>
#include <kernel/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

struct register_set
{
    // NOTE: The function get_registers(), below, requires that these fields
    // be in the reverse order that the registers are pushed onto the stack
    // by the PUSHA instruction.
    u32 di;
    u32 si;
    u32 bp;
    u32 sp;
    u32 b;
    u32 d;
    u32 c;
    u32 a;
};

static ALWAYS_INLINE struct register_set get_registers(void)
{
    // Push all general-purpose registers onto the stack, then return a
    // pointer to their location on the stack, and allow the compiler to
    // dereference it as it wants.
    struct register_set *regset;
    ASM_VOLATILE(
        "pusha;"
        "mov    %0, esp;"
        "add    esp, %1;":
        "=g"(regset):
        "Z"(sizeof(struct register_set))
    );
    return *regset;
}

static ALWAYS_INLINE u32 get_eflags(void)
{
    u32 result;
    ASM_VOLATILE(
        "pushf  \n\t"
        "pop %0 \n\t":
        "=g"(result)
    );
    return result;
}

struct cpustat
{
    struct register_set regset;
    u32                 eflags;
};

static ALWAYS_INLINE struct cpustat collect_cpustat(void)
{
    struct cpustat cs;
    cs.regset = get_registers();
    cs.eflags = get_eflags();
    return cs;
}

int dump_cpustat(char *buffer, size_t buffer_size, const struct cpustat cs);

#ifdef __cplusplus
}
#endif

#endif /* _INC_KERNEL_ASM_CPUSTAT */
