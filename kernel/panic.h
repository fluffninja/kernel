#ifndef _INC_PANIC
#define _INC_PANIC 1

#include <sys/asm.h>

void panic_set_use_bsod(int use_bsod);
void NO_RETURN paniccs(const struct cpustat cs, const char *fmt, ...);
void NO_RETURN panic(const char *fmt, ...);

// Panic if the given predicate is not met, outputting a message
#define KASSERTV(PRED, MSG)                     \
    if (!(PRED)) {                              \
        struct cpustat cs = collect_cpustat();  \
        paniccs(cs,                             \
            "kernel assertion\n"                \
            " site: %s:%d\n"                    \
            " pred: %s\n"                       \
            " msg:  %s\n",                      \
            __FILE__, __LINE__, #PRED, MSG);    \
    }

// Panic if the given predicate is not met
#define KASSERT(PRED)   KASSERTV(PRED, "nil")

#endif /* _INC_PANIC */
