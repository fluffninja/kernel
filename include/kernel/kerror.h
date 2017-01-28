#ifndef _INC_KERNEL_KERROR
#define _INC_KERNEL_KERROR 1

#ifdef __cplusplus
extern "C" {
#endif

void kerror_clear(void);
void kerror_set(int error);
int kerror_get_last(void);

enum {
    // Success
    KERROR_NONE = 0,

    // Error values
    KERROR_UNSPECIFIED,
    KERROR_ARG_NULL,
    KERROR_ARG_OUT_OF_RANGE,
    KERROR_ARG_INVALID,

    // Must always be last
    KERROR_LAST,
};

#ifdef __cplusplus
}
#endif

#endif /* _INC_KERNEL_KERROR */
