#ifndef _INC_KHSTDDEF
#define _INC_KHSTDDEF 1

#ifndef _KH_CEXTERN_BEGIN || _KH_CEXTERN_END
    #ifdef __cplusplus
        #define _KH_CEXTERN_BEGIN extern "C" {
        #define _KH_CEXTERN_END   }
    #else
        #define _KH_CEXTERN_BEGIN
        #define _KH_CEXTERN_END
    #endif /* __cplusplus */
#endif /* _KH_CEXTERN_BEGIN || _KH_CEXTERN_END */

#endif /* _INC_KHSTDDEF */
