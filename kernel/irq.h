#ifndef _INC_PICIRQ
#define _INC_PICIRQ 1

#include <sys/asm.h>

#define IRQ_PIC_MASTER_IDT_OFFSET   0x20
#define IRQ_PIC_SLAVE_IDT_OFFSET    0x28

typedef int (*irq_hook_t)(int irqnum);

int irq_init(void);
int irq_is_valid_irqnum(int irqnum);
int irq_has_hook(int irqnum);
int irq_set_hook(int irqnum, irq_hook_t hookfn);
int irq_remove_hook(int irqnum);
int irq_call_hook(int irqnum);
int irq_set_enabled(int irqnum, int enabled);
int irq_done(int irqnum);

#endif /* _INC_PICIRQ */
