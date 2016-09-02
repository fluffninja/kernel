#ifndef _INC_ISR
#define _INC_ISR 1

#include <sys/asm.h>

// Create/get the name of the handler function
#define ISR_HANDLER(HNAME)  __handle_##HNAME

// Template for ISR handler functions
#define ISR_DEF_HANDLER_BASE(HNAME, HBODY)  \
    void HNAME(void)                        \
    {                                       \
        ASM_VOLATILE(                       \
            "pusha"                         \
        );                                  \
        {                                   \
            HBODY                           \
        }                                   \
        ASM_VOLATILE(                       \
            "popa   \n\t"                   \
            "leave  \n\t"                   \
            "iret   \n\t"                   \
        );                                  \
    }

// TODO: add other int data to def handler

// Define an ISR handler routine that recieves the set of register values as
// they were before the interrupt occurred.
#define ISR_DEF_HANDLER(ISRNAME)                    \
    ISR_DEF_HANDLER_BASE(ISR_HANDLER(ISRNAME),      \
    {                                               \
        extern void ISRNAME(struct register_set);   \
        struct register_set regset;                 \
        regset = get_registers();                   \
        ISRNAME(regset);                            \
    });

// Define an ISR handler routine (does not receive register values)
// Note that pusha and popa are still used to prevent register cobbering by
// handler
#define ISR_DEF_NOREG_HANDLER(ISRNAME)          \
    ISR_DEF_HANDLER_BASE(ISR_HANDLER(ISRNAME),  \
    {                                           \
        extern void ISRNAME(void);              \
        ISRNAME();                              \
    });

int isr_init(void);
int isr_set_handler(int index, void (*handler)(void));

#endif /* _INC_ISR */
