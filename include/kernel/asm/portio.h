#ifndef _INC_KERNEL_ASM_PORTIO
#define _INC_KERNEL_ASM_PORTIO 1

#include <kernel/kernel.h>
#include <kernel/types.h>
#include <kernel/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

// Port used by portwait(), which _should_ be an unused port.
#define PORTWAIT_PORT 0x80

// Write a byte to the given port
static ALWAYS_INLINE void outportb(u16 port, u8 value)
{
    ASM_VOLATILE(
        "out %1, %0"::
        "a"  (value),       // Value in A
        "dN" (port)         // Port Number Constant in D
    );
}

// Write a word to the given port
static ALWAYS_INLINE void outportw(u16 port, u16 value)
{
    ASM_VOLATILE(
        "out %1, %0"::
        "a"  (value),
        "dN" (port)
    );
}

// Write a double-word to the given port
static ALWAYS_INLINE void outportl(u16 port, u32 value)
{
    ASM_VOLATILE(
        "out %1, %0"::
        "a"  (value),
        "dN" (port)
    );
}

// Read a byte from the given port
static ALWAYS_INLINE u8 inportb(u16 port)
{
    u8 value;
    ASM_VOLATILE(
        "in %0, %1":
        "=a"(value):      // Value of A
        "dN"(port)       // Port Number Constant in D
    );
    return value;
}

// Read a word from the given port
static ALWAYS_INLINE u16 inportw(u16 port)
{
    u16 value;
    ASM_VOLATILE(
        "in %0, %1":
        "=a"(value):
        "dN"(port)
    );
    return value;
}

// Read a double-word from the given port
static ALWAYS_INLINE u32 inportl(u16 port)
{
    u32 value;
    ASM_VOLATILE(
        "in %0, %1":
        "=a"(value):
        "dN"(port)
    );
    return value;
}

static ALWAYS_INLINE void portwait(void)
{
    // Reference: http://www.makelinux.net/ldd3/chp-9-sect-2
    // Output to an unused port to make sure any previous port-outputs have
    // had time to be processed.
    outportb(PORTWAIT_PORT, 0);
}

#ifdef __cplusplus
}
#endif

#endif /* _INC_KERNEL_ASM_PORTIO */
