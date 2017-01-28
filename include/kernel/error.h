#ifndef _INC_KERNEL_ERROR
#define _INC_KERNEL_ERROR 1

enum {
    // Success
    KERROR_OK = 0,

    // Failcode
    KERROR_ARG_NULL,
    KERROR_ARG_OUT_OF_RANGE,
    KERROR_ARG_INVALID,
};

#endif /* _INC_KERNEL_ERROR */
