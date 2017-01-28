#ifndef _INC_SYSCALL
#define _INC_SYSCALL 1

#include <kernel/compiler.h>
#include <kernel/types.h>

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

static ALWAYS_INLINE void __syscall1(u32 a)
{
    ASM_VOLATILE(
        "int %0"::
        "N"(SYSCALL_IDT_INDEX),
        "a"(a):
        "cc",
        "memory"
    );
}

static ALWAYS_INLINE void __syscall2(u32 a, u32 b)
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

static ALWAYS_INLINE void __syscall3(u32 a, u32 b, u32 c)
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

static ALWAYS_INLINE void __syscall4(u32 a, u32 b, u32 c, u32 d)
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
