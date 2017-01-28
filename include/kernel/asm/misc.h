#ifndef _INC_KERNEL_ASM_MISC
#define _INC_KERNEL_ASM_MISC 1

#include <kernel/kernel.h>
#include <kernel/types.h>
#include <kernel/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

// Disable interrupts
static ALWAYS_INLINE void cli(void)
{
    ASM("cli");
}

// Enable interrupts
static ALWAYS_INLINE void sti(void)
{
    ASM("sti");
}

// Halt until next interrupt
static ALWAYS_INLINE void hlt(void)
{
    ASM("hlt");
}

// Get clock-cycles since boot via RDTSC (Read Time-stamp counter)
static ALWAYS_INLINE u64 rdtsc(void)
{
    u64 value;
    ASM(
        "rdtsc":
        "=A"(value)
    );
    return value;
}

// Write model-specific register
static ALWAYS_INLINE void wrmsr(u32 reg, u64 value)
{
    ASM(
        "wrmsr"::
        "c"(reg),
        "A"(value)
    );
}

// Read model-specific register
static ALWAYS_INLINE u64 rdmsr(u32 reg)
{
    u64 value;
    ASM(
        "rdmsr":
        "=A"(value):
        "c"(reg)
    );
    return value;
}

static ALWAYS_INLINE void ud2(void)
{
    ASM_VOLATILE("ud2");
}

#ifdef __cplusplus
}
#endif

#endif /* _INC_KERNEL_ASM_MISC */
