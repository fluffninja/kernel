// C/C++ Compiler Compatibility

#ifndef _INC_CCCOMPAT
#define _INC_CCCOMPAT 1

#ifdef __GNUC__
    #define _COMP_GCC
#endif /* __GNUC__ */

#ifdef _MSC_VER
    #define _COMP_MSVC
#endif /* _MSC_VER */

#ifdef _COMP_GCC
    #define INLINE          __inline__
    #define ALWAYS_INLINE   __inline__ __attribute__((always_inline))
    #define NAKED           __attribute__((naked))
    #define NO_INLINE       __attribute__((noinline))
    #define NO_RETURN       __attribute__((noreturn))
    #define NO_THROW        __attribute__((nothrow))
    #define NO_OPTIMISE     __attribute__((optimize("O0")))
    #define CDECL           __attribute__((__cdecl__))
    #define ALIGN(A)        __attribute__((aligned(A)))
    #define START_PACK      
    #define END_PACK        __attribute__((packed))
    #define ASM             __asm__
    #define ASM_VOLATILE    __asm__ volatile
    #define ASM_GOTO        __asm__ goto
#else
    #define INLINE
    #define ALWAYS_INLINE
    #define NAKED
    #define NO_INLINE
    #define NO_RETURN
    #define NO_THROW
    #define NO_OPTIMISE
    #define CDECL
    #define ALIGN(A)
    #define START_PACK      
    #define END_PACK
    #define ASM     
    #define ASM_VOLATILE
    #define ASM_GOTO
#endif

#endif /* _INC_CCCOMPAT */
