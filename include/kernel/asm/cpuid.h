#ifndef _INC_KERNEL_ASM_CPUID
#define _INC_KERNEL_ASM_CPUID 1

#include <kernel/kernel.h>
#include <kernel/compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

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


// Result fields of CPUID
struct cpuid_result
{
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
};

INLINE struct cpuid_result cpuid(uint32_t query)
{
    struct cpuid_result result;
    ASM(
        "cpuid":
        "=a"(result.a),
        "=b"(result.b),
        "=c"(result.c),
        "=d"(result.d):
        "a"(query)
    );
    return result;
}

#ifdef __cplusplus
}
#endif

#endif /* _INC_KERNEL_ASM_CPUID */
