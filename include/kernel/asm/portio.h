#ifndef _INC_KERNEL_ASM_PORTIO
#define _INC_KERNEL_ASM_PORTIO 1

#include <cccompat.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Port used by portwait(), which _should_ be an unused port.
#define PORTWAIT_PORT 0x80

// Write a byte to the given port
static ALWAYS_INLINE void outportb(uint16_t port, uint8_t value)
{
    ASM_VOLATILE(
        "outb %1, %0"::
        "a"  (value),       // Value in A
        "dN" (port)         // Port Number Constant in D
    );
}

// Write a word to the given port
static ALWAYS_INLINE void outportw(uint16_t port, uint16_t value)
{
    ASM_VOLATILE(
        "outw %1, %0"::
        "a"  (value),
        "dN" (port)
    );
}

// Write a double-word to the given port
static ALWAYS_INLINE void outportl(uint16_t port, uint32_t value)
{
    ASM_VOLATILE(
        "outl %1, %0"::
        "a"  (value),
        "dN" (port)
    );
}

// Read a byte from the given port
static ALWAYS_INLINE uint8_t inportb(uint16_t port)
{
    uint8_t value;
    ASM_VOLATILE(
        "inb %0, %1":
        "=a"(value):      // Value of A
        "dN"(port)       // Port Number Constant in D
    );
    return value;
}

// Read a word from the given port
static ALWAYS_INLINE uint16_t inportw(uint16_t port)
{
    uint16_t value;
    ASM_VOLATILE(
        "inw %0, %1":
        "=a"(value):
        "dN"(port)
    );
    return value;
}

// Read a double-word from the given port
static ALWAYS_INLINE uint32_t inportl(uint16_t port)
{
    uint32_t value;
    ASM_VOLATILE(
        "inl %0, %1":
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

#endif /* _INC_KERNEL_PORTIO */
