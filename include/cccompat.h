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
    #define INLINE          extern inline
    #define NAKED           __attribute__((naked))
    #define NO_INLINE       __attribute__((noinline))
    #define NO_RETURN       __attribute__((noreturn))
    #define NO_THROW        __attribute__((nothrow))
    #define CDECL           __attribute__((__cdecl__))
    #define ALIGN(A)        __attribute__((aligned(A)))
#elif _COMP_MSVC
    #define INLINE          __declspec(inline)
    #define NAKED           __declspec(naked)
    #define NO_INLINE       __declspec(noinline)
    #define NO_RETURN       __declspec(noreturn)
    #define NO_THROW        __declspec(nothrow)
    #define CDECL           __cdecl
    #defome ALIGN(A)        __declspec(align(A))
#else
    #define INLINE
    #define NAKED
    #define NO_INLINE
    #define NO_RETURN
    #define NO_THROW
    #define CDECL
    #define ALIGN(A)
#endif

#endif /* _INC_CCCOMPAT */
