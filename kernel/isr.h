#ifndef _INC_ISR
#define _INC_ISR 1

#include <kernel/asm/cpustat.h>
#include <cccompat.h>

// Contains functions and utilities for registering handlers of interrupt
// service routines (ISRs). These are functions that will be called when a
// given interrupt - CPU, hardware, or software - is invoked.

// Due to the fact that interrupts have an abnormal calling convention,
// registerred ISRs must be registered via a 'handler'. A handler is a wrapper
// function a given ISR. One must be defined for each ISR.

// ISRs are given
struct isr_params {
    struct cpustat cs;
};

// Gets the name of the ISR handler function
#define ISR_HANDLER(ISRFUNC)    __handle_##ISRFUNC

#define ISR_HANDLER_ATTR    /* None */

// Template for ISR handler functions
#define __ISR_HOOK_HANDLER_BASE(ISRFUNC, ISRBODY)       \
    void ISR_HANDLER_ATTR ISRFUNC(void)                 \
    {                                                   \
        ASM_VOLATILE(                                   \
            "pusha"                                     \
        );                                              \
        {                                               \
            ISRBODY                                     \
        }                                               \
        ASM_VOLATILE(                                   \
            "popa   \n\t"                               \
            "leave  \n\t"                               \
            "iret   \n\t"                               \
        );                                              \
    }

// Standard ISR handler function
// Gives the ISR an isr_params object, which contains information about the
// state of the environment when the ISR was invoked. Note that this involves
// a small amount of prior processing.
#define ISR_DEF_HANDLER(ISRFUNC)                        \
    __ISR_HOOK_HANDLER_BASE(ISR_HANDLER(ISRFUNC),       \
    {                                                   \
        extern void ISRFUNC(struct isr_params params);  \
        struct isr_params params;                       \
        params.cs = collect_cpustat();                  \
        ISRFUNC(params);                                \
    });

// Parameterless ISR handler function
// Performs no prior processing to acquire information about the ISR calling
// environment. This makes it good for hardware interrupts.
#define ISR_DEF_HANDLER_NOPARAMS(ISRFUNC)               \
    __ISR_HOOK_HANDLER_BASE(ISR_HANDLER(ISRFUNC),       \
    {                                                   \
        extern void ISRFUNC(void);                      \
        ISRFUNC();                                      \
    });

// Initialise the ISR handling system, registering ISRs for all CPU exceptions
int isr_init(void);

// Sets the handler function for the ISR of a given number - the ISR number
// being its index in the IDT, aka the interrupt number.
int isr_set_handler(int isrnum, void (*handler)(void));

#endif /* _INC_ISR */
