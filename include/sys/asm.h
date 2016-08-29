// Useful Links:
// https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html
// https://gcc.gnu.org/onlinedocs/gcc/Simple-Constraints.html

#ifndef _INC_ASM
#define _INC_ASM 1

#include <cccompat.h>
#include <stdint.h>

// Write a byte to the given port
ALWAYS_INLINE void
outb(uint16_t port, uint8_t value)
{
    ASM_VOLATILE(
        "outb %0, %1"::
        "a"  (value),       // Value in A
        "dN" (port)         // Port Number Constant in D
    );
}

// Write a word to the given port
ALWAYS_INLINE void
outw(uint16_t port, uint16_t value)
{
    ASM_VOLATILE(
        "outw %0, %1"::
        "a"  (value),
        "dN" (port)
    );
}

// Write a double-word to the given port
ALWAYS_INLINE void
outl(uint16_t port, uint32_t value)
{
    ASM_VOLATILE(
        "outl %0, %1"::
        "a"  (value),
        "dN" (port)
    );
}

// Read a byte from the given port
ALWAYS_INLINE uint8_t
inb(uint16_t port)
{
    uint8_t value;
    ASM_VOLATILE(
        "inb %1, %0":
        "=a"(value):      // Value of A
        "dN"(port)       // Port Number Constant in D
    );
    return value;
}

// Read a word from the given port
ALWAYS_INLINE uint16_t
inw(uint16_t port)
{
    uint16_t value;
    ASM_VOLATILE(
        "inw %1, %0":
        "=a"(value):
        "dN"(port)
    );
    return value;
}

// Read a double-word from the given port
ALWAYS_INLINE uint32_t
inl(uint16_t port)
{
    uint32_t value;
    ASM_VOLATILE(
        "inl %1, %0":
        "=a"(value):
        "dN"(port)
    );
    return value;
}

// Disable interrupts
ALWAYS_INLINE void cli(void)
{
    ASM_VOLATILE("cli");
}

// Enable interrupts
ALWAYS_INLINE void sti(void)
{
    ASM_VOLATILE("sti");
}

ALWAYS_INLINE uint32_t
get_eflags(void)
{
    uint32_t value;
    ASM_VOLATILE(
        "pushfl     \n\t"
        "popl   %0  \n\t":
        "=a"(value)
    );
    return value;
}

ALWAYS_INLINE void
set_eflags(uint32_t value)
{
    ASM_VOLATILE(
        "pushl  %0  \n\t"
        "popfl      \n\t"::
        "a"(value)
    );
}

PACKED struct gdt_entry
{

};

// 48-bit GDT descriptor, for use with lgdt and sgdt
PACKED struct gdt_descriptor
{
    uint16_t    size;
    void        *base;
};

// Load GDT
ALWAYS_INLINE void
lgdt(struct gdt_descriptor *desc)
{
    ASM_VOLATILE(
        "lgdt %0"::
        "m"(desc)
    );
}

// Store GDT
ALWAYS_INLINE void
sgdt(struct gdt_descriptor *desc)
{
    ASM_VOLATILE(
        "sgdt %0":
        "=m"(desc)
    );
}

PACKED struct idt_entry
{

};

// 48-bit IDT descriptor, for use with lidt and sidt
PACKED struct idt_descriptor
{
    uint16_t    size;
    void        *base;
};

// Load IDT
ALWAYS_INLINE void
lidt(struct idt_descriptor *desc)
{
    ASM_VOLATILE(
        "lidt %0"::
        "m"(desc)
    );
}

// Store IDT
ALWAYS_INLINE void
sidt(struct idt_descriptor *desc)
{
    ASM_VOLATILE(
        "sidt %0":
        "=m"(desc)
    );
}

// Get clock-cycles since boot via RDTSC (Read Time-stamp counter)
ALWAYS_INLINE uint64_t
rdtsc(void)
{
    uint64_t value;
    ASM_VOLATILE(
        "rdtsc":
        "=A"(value)
    );
    return value;
}

// Write model-specific register
ALWAYS_INLINE void
wrmsr(uint32_t reg, uint64_t value)
{
    ASM_VOLATILE(
        "wrmsr"::
        "c"(reg),
        "A"(value)
    );
}

// Read model-specific register
ALWAYS_INLINE uint64_t
rdmsr(uint32_t reg)
{
    uint64_t value;
    ASM_VOLATILE(
        "rdmsr":
        "=A"(value):
        "c"(reg)
    );
    return value;
}

// Result fields of CPUID
struct cpuid_result
{
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
};

ALWAYS_INLINE struct cpuid_result
cpuid(uint32_t query)
{
    struct cpuid_result result;
    ASM_VOLATILE(
        "cpuid":
        "=a"(result.a),
        "=b"(result.b),
        "=c"(result.c),
        "=d"(result.d):
        "a"(query)
    );
    return result;
}

#define BITFLAG(N)                      (1UL << (N))

#define CPUID_QUERY_VENDOR_ID           0x00
#define CPUID_QUERY_FEATURES            0x01
#define CPUID_QUERY_EXTENDED_FEATURES   0x07
#define CPUID_QUERY_HIGHEST_QUERY       0x80000000
#define CPUID_QUERY_EXTENDED_FEATURES_2 0x80000001
#define CPUID_QUERY_BRAND_STRING_1      0x80000002
#define CPUID_QUERY_BRAND_STRING_2      0x80000003
#define CPUID_QUERY_BRAND_STRING_3      0x80000004   
#define CPUID_QUERY_ADVANCED_POWER_INFO 0x80000007
#define CPUID_QUERY_HIGHEST_VIRT_ADDR   0x80000008

#define CPUID_FEATURE_FPU               BITFLAG(0)  // On-board x87 FPU
#define CPUID_FEATURE_VME               BITFLAG(1)  // Virtual 8086 Extensions
#define CPUID_FEATURE_TSC               BITFLAG(4)  // Time-stamp Counter
#define CPUID_FEATURE_MSR               BITFLAG(5)  // Model-specific Registers
#define CPUID_FEATURE_MCE               BITFLAG(7)  // Machine-check Exceptions
#define CPUID_FEATURE_APIC              BITFLAG(9)  // Advanced PIC
#define CPUID_FEATURE_SYSENTER_SYSEXIT  BITFLAG(11) // SYSENTER and SYSEXIT

#endif /* _INC_ASM */
