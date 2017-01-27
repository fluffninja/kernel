#ifndef _INC_KERNEL_KLOG
#define _INC_KERNEL_KLOG 1

#ifdef __cplusplus
extern "C" {
#endif

int klog_printf(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* _INC_KERNEL_KLOG */
