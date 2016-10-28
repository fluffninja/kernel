#include "syscall.h"
#include "panic.h"
#include "kio.h"
#include "isr.h"

static ISR_DEF_HANDLER(isr_syscall);

void isr_syscall(struct isr_params params)
{
    paniccs(params.cs, "syscall unimplemented\n");
}

int syscall_init(void)
{
    if (isr_set_handler(SYSCALL_IDT_INDEX, ISR_HANDLER(isr_syscall))) {
        kprintf("syscall: failed to register isr %#2x\n", SYSCALL_IDT_INDEX);
        return 1;
    }

    kprintf("syscall: registered isr %#2x\n", SYSCALL_IDT_INDEX);
    return 0;
}
