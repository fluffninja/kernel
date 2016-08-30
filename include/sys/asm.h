// Useful Links:
// https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html
// https://gcc.gnu.org/onlinedocs/gcc/Simple-Constraints.html

#ifndef _INC_ASM
#define _INC_ASM 1

#include <cccompat.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif 

// Write a byte to the given port
ALWAYS_INLINE void
outportb(uint16_t port, uint8_t value)
{
    ASM_VOLATILE(
        "outb %1, %0"::
        "a"  (value),       // Value in A
        "dN" (port)         // Port Number Constant in D
    );
}

// Write a word to the given port
ALWAYS_INLINE void
outportw(uint16_t port, uint16_t value)
{
    ASM_VOLATILE(
        "outw %1, %0"::
        "a"  (value),
        "dN" (port)
    );
}

// Write a double-word to the given port
ALWAYS_INLINE void
outportl(uint16_t port, uint32_t value)
{
    ASM_VOLATILE(
        "outl %1, %0"::
        "a"  (value),
        "dN" (port)
    );
}

// Read a byte from the given port
ALWAYS_INLINE uint8_t
inportb(uint16_t port)
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
ALWAYS_INLINE uint16_t
inportw(uint16_t port)
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
ALWAYS_INLINE uint32_t
inportl(uint16_t port)
{
    uint32_t value;
    ASM_VOLATILE(
        "inl %0, %1":
        "=a"(value):
        "dN"(port)
    );
    return value;
}

ALWAYS_INLINE void
portwait(void)
{
    // Reference: http://www.makelinux.net/ldd3/chp-9-sect-2
    // Output to an unused port to make sure any previous port-outputs have
    // had time to be processed. 0x80 'should' be unused.
    outportb(0x80, 0);
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

START_PACK struct gdt_entry
{
} END_PACK;

// 48-bit GDT descriptor, for use with lgdt and sgdt
START_PACK struct gdt_descriptor
{
    uint16_t            size;
    struct gdt_entry    *base;
} END_PACK;

// IDT Entry Flags
#define IDT_PRESENT             0x80 // Interrupt entry should be used    
#define IDT_PRIVILEGE_0         0x00 // Highest ring that can use interrupt
#define IDT_PRIVILEGE_1         0x20 // ^
#define IDT_PRIVILEGE_2         0x40 // ^
#define IDT_PRIVILEGE_3         0x60 // ^
#define IDT_GATE_TASK_32        0x05 // This is a 32-bit task gate
#define IDT_GATE_INTERRUPT_16   0x06 // This io a 16-bit interrupt gate
#define IDT_GATE_TRAP_16        0x07 // This is a 16-bit task gate
#define IDT_GATE_INTERRUPT_32   0x0e // This is a 32-bit interrupt gate
#define IDT_GATE_TRAP_32        0x0f // This is a 32-bit trap gate

START_PACK struct idt_entry
{
    uint16_t handler_offset_00_15;  // Offset of handler code in GDT segment
    uint16_t gdt_handler_selector;  // GDT segment - as table selector
    uint8_t  _reserved0;            // Reserved byte
    uint8_t  flags;                 // Flags detailing type and attributes
    uint16_t handler_offset_16_32;  // Upper word of handler offset
} END_PACK;

#define DEF_IDT_ENTRY(HANDLER_OFFSET, GDT_SELECTOR, FLAGS)      \
    {                                                           \
        (uint16_t) (((uint32_t) (HANDLER_OFFSET)) & 0xffff),    \
        (uint16_t) (GDT_SELECTOR),                              \
        0,                                                      \
        (uint8_t) (FLAGS),                                      \
        (uint16_t) (((uint32_t) (HANDLER_OFFSET)) >> 16),       \
    }

// 48-bit IDT descriptor, for use with lidt and sidt
START_PACK struct idt_descriptor
{
    uint16_t            size;
    struct idt_entry    *base;
} END_PACK;

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

#ifdef __cplusplus
}
#endif 

#endif /* _INC_ASM */
