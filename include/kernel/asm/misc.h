#ifndef _INC_KERNEL_ASM_MISC
#define _INC_KERNEL_ASM_MISC 1

#include <cccompat.h>
#include <stdint.h>

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
static ALWAYS_INLINE uint64_t rdtsc(void)
{
    uint64_t value;
    ASM(
        "rdtsc":
        "=A"(value)
    );
    return value;
}

// Write model-specific register
static ALWAYS_INLINE void wrmsr(uint32_t reg, uint64_t value)
{
    ASM(
        "wrmsr"::
        "c"(reg),
        "A"(value)
    );
}

// Read model-specific register
static ALWAYS_INLINE uint64_t rdmsr(uint32_t reg)
{
    uint64_t value;
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
