#ifndef _INC_KERNEL_KERROR
#define _INC_KERNEL_KERROR 1

#ifdef __cplusplus
extern "C" {
#endif

void kerror_set_last(int error);
void kerror_clear_last(void);
int kerror_pop_last(void);
int kerror_peek_last(void);

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
