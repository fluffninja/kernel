#include <kernel/klog.h>

#include "syscall.h"
#include "isr.h"
#include "../panic.h"

static ISR_DEF_HANDLER(isr_syscall);

void isr_syscall(struct isr_params params)
{
    paniccs(params.cs, "syscall unimplemented\n");
}

int syscall_init(void)
{
    if (isr_set_handler(SYSCALL_IDT_INDEX, ISR_HANDLER(isr_syscall))) {
        klog_printf("syscall: failed to register isr %#2x\n", SYSCALL_IDT_INDEX);
        return 1;
    }

    klog_printf("syscall: registered isr %#2x\n", SYSCALL_IDT_INDEX);

    return 0;
}
