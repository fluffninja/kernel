#include <stddef.h>

#include "boot.h"

struct kernel_boot_params *
boot_get_params(intptr_t addr)
{
    struct kernel_boot_params *params = (struct kernel_boot_params *) addr;

    if (params->signature == BOOT_PARAM_BLOCK_SIG) {
        return params;
    } else {
        return NULL;
    }
}
