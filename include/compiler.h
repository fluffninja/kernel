#ifndef _INC_COMPILER
#define _INC_COMPILER 1

#ifdef __GNUC__
    #define _COMP_GCC
#endif /* __GNUC__ */

#ifdef _MSC_VER
    #define _COMP_MSVC
#endif /* _MSC_VER */

#ifdef _COMP_GCC
    #define INLINE          inline
    #define ALWAYS_INLINE   inline __attribute__((__always_inline__))
    #define NAKED           __attribute__((__naked__))
    #define NO_INLINE       __attribute__((__noinline__))
    #define NO_RETURN       __attribute__((__noreturn__))
    #define NO_THROW        __attribute__((__nothrow__))
    #define NO_OPTIMISE     __attribute__((__optimize__("O0")))
    #define CDECL           __attribute__((__cdecl__))
    #define ALIGN(A)        __attribute__((__aligned__(A)))
    #define BEGIN_PACK
    #define END_PACK        __attribute__((__packed__))
    #define ASM             __asm__
    #define ASM_VOLATILE    __asm__ volatile
    #define ASM_GOTO        __asm__ goto
    #define NO_REMOVE       __attribute__((__used__))
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
    #define BEGIN_PACK
    #define END_PACK
    #define ASM
    #define ASM_VOLATILE
    #define ASM_GOTO
    #define NO_REMOVE
#endif

#endif /* _INC_COMPILER */
