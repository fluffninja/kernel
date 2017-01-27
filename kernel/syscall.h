#ifndef _INC_SYSCALL
#define _INC_SYSCALL 1

#include <kernel/compiler.h>
#include <stdint.h>

#define SYSCALL_IDT_INDEX 0x40

static ALWAYS_INLINE void __syscall(void)
{
    ASM_VOLATILE(
        "int %0"::
        "N"(SYSCALL_IDT_INDEX):
        "cc",
        "memory"
    );
}

static ALWAYS_INLINE void __syscall1(uint32_t a)
{
    ASM_VOLATILE(
        "int %0"::
        "N"(SYSCALL_IDT_INDEX),
        "a"(a):
        "cc",
        "memory"
    );
}

static ALWAYS_INLINE void __syscall2(uint32_t a, uint32_t b)
{
    ASM_VOLATILE(
        "int %0"::
        "N"(SYSCALL_IDT_INDEX),
        "a"(a),
        "b"(b):
        "cc",
        "memory"
    );
}

static ALWAYS_INLINE void __syscall3(uint32_t a, uint32_t b, uint32_t c)
{
    ASM_VOLATILE(
        "int %0"::
        "N"(SYSCALL_IDT_INDEX),
        "a"(a),
        "b"(b),
        "c"(c):
        "cc",
        "memory"
    );
}

static ALWAYS_INLINE void __syscall4(uint32_t a, uint32_t b, uint32_t c,
    uint32_t d)
{
    ASM_VOLATILE(
        "int %0"::
        "N"(SYSCALL_IDT_INDEX),
        "a"(a),
        "b"(b),
        "c"(c),
        "d"(d):
        "cc",
        "memory"
    );
}

int syscall_init(void);

#endif /* _INC_SYSCALL */
