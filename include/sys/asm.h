#ifndef _INC_ASM
#define _INC_ASM 1

// Useful Links:
// https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html
// https://gcc.gnu.org/onlinedocs/gcc/Simple-Constraints.html

#include <cccompat.h>
#include <stdint.h>

INLINE void
outb(uint16_t port, uint8_t value)
{
    __asm volatile(
        "outb %0, %1" : :
        "a"  (value),       // Value in A
        "dN" (port)         // Port Number Constant in D
    );
}

INLINE void
outw(uint16_t port, uint16_t value)
{
    __asm volatile(
        "outw %0, %1" : :
        "a"  (value),
        "dN" (port)
    );
}

INLINE uint8_t
inb(uint16_t port)
{
    uint8_t value;
    __asm volatile(
        "inb %1, %0"
        : "=a" (value)      // Value of A
        : "dN" (port)       // Port Number Constant in D
    );
    return value;
}

INLINE uint16_t
inw(uint16_t port)
{
    uint16_t value;
    __asm volatile(
        "inw %1, %0"
        : "=a" (value)
        : "dN" (port)
    );
    return value;
}

#endif /* _INC_ASM */
