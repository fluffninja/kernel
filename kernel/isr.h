#ifndef _INC_ISR
#define _INC_ISR 1

#include <sys/asm.h>

// Create/get the name of the handler function
#define ISR_HANDLER(FUNC)   __handle_##FUNC

// TODO: Split ISR_DEF_HANDLER into separate handlers for 'functional
// interrupts' (eg syscall) vs. exception handlers (which need all flags and
// registers, including control registers) vs. hardware handlers (for which 
// registers aren't needed).

// Define an ISR handler routine
#define ISR_DEF_HANDLER(FUNC)                   \
    void ISR_HANDLER(FUNC)(void) {              \
        extern void FUNC(struct register_set);  \
        ASM_VOLATILE("pusha");                  \
        (void) FUNC(get_registers());           \
        ASM_VOLATILE(                           \
            "popa   \n\t"                       \
            "leave  \n\t"                       \
            "iret   \n\t"                       \
        );                                      \
    }                                       

int isr_init(void);
int isr_set_handler(int index, void (*handler)(void));

#endif /* _INC_ISR */
