#ifndef _INC_CPUSTAT
#define _INC_CPUSTAT 1

#include <compiler.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct register_set
{
    // NOTE: The function get_registers(), below, requires that these fields
    // be in the reverse order that the registers are pushed onto the stack
    // by the PUSHA instruction.
    uint32_t di;
    uint32_t si;
    uint32_t bp;
    uint32_t sp;
    uint32_t b;
    uint32_t d;
    uint32_t c;
    uint32_t a;
};

static ALWAYS_INLINE struct register_set get_registers(void)
{
    // Push all general-purpose registers onto the stack, then return a
    // pointer to their location on the stack, and allow the compiler to
    // dereference it as it wants.
    struct register_set *regset;
    ASM_VOLATILE(
        "pusha              \n\t"
        "mov    %0, esp     \n\t"
        "add    esp, %1     \n\t":
        "=g"(regset):
        "Z"(sizeof(struct register_set))
    );
    return *regset;
}

static ALWAYS_INLINE uint32_t get_eflags(void)
{
    uint32_t result;
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
    uint32_t            eflags;
};

static ALWAYS_INLINE struct cpustat collect_cpustat(void)
{
    struct cpustat cs;
    cs.regset = get_registers();
    cs.eflags = get_eflags();
    return cs;
}

#ifdef __cplusplus
}
#endif

#endif /* _INC_CPUSTAT */
