#ifndef _INC_BOOT
#define _INC_BOOT 1

#include <stdint.h>
#include <compiler.h>

// The kernel boot parameter block allows the bootloader to hand-over binary
// information to the kernel during the boot process. Its presence and validity
// are confirmed via a signature.
BEGIN_PACK struct kernel_boot_params {
    uint16_t    signature;
    uint8_t     cursor_x;
    uint8_t     cursor_y;
} END_PACK;

// These values MUST agree with the values in boot/boot.asm
#define BOOT_PARAM_BLOCK_ADDR   0x7000
#define BOOT_PARAM_BLOCK_SIG    0xc33c

// Acquires the kernel boot parameter block from its location in memory.
// addr:
// Used to specify the location at which to look for a boot parameter block.
// Return:
// If the given address does not contain a valid parameter block, then a null
// pointer is returned.
struct kernel_boot_params *boot_get_params(intptr_t addr);

#endif /* _INC_BOOT */
