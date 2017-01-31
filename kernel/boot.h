#ifndef _INC_BOOT
#define _INC_BOOT 1

#include <kernel/kernel.h>
#include <kernel/types.h>
#include <kernel/compiler.h>

// The kernel boot parameter block allows the bootloader to hand-over binary
// information to the kernel during the boot process. Its presence and validity
// are confirmed via a signature.
BEGIN_PACK struct kernel_boot_params {
    u16 signature;
    u8  cursor_x;
    u8  cursor_y;
} END_PACK;

// These values MUST agree with the values in boot/boot.asm
#define BOOT_PARAM_BLOCK_ADDR   ((void *) 0x00007000)
#define BOOT_PARAM_BLOCK_SIG    0x0000c33c

// Acquires the kernel boot parameter block from its location in memory.
// addr:
// Used to specify the location at which to look for a boot parameter block.
// Return:
// If the given address does not contain a valid parameter block, then a null
// pointer is returned.
struct kernel_boot_params *boot_get_params(void *addr);

#endif /* _INC_BOOT */
