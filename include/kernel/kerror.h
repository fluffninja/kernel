#ifndef _INC_KERNEL_ERROR
#define _INC_KERNEL_ERROR 1

void kerror_clear(void);
void kerror_set(int error);
int kerror_get_last(void);

enum {
    // Success
    KERROR_NONE = 0,

    KERROR_UNSPECIFIED,
    KERROR_ARG_NULL,
    KERROR_ARG_OUT_OF_RANGE,
    KERROR_ARG_INVALID,

    // Must always be last
    KERROR_LAST,
};

#endif /* _INC_KERNEL_ERROR */
