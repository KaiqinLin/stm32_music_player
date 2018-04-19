#ifndef __COMMON_H
#define __COMMON_H
#include <stdlib.h>
#include <stdint.h>
#include "./usart/bsp_usart.h"

#define DEBUG
#ifdef DEBUG
  #define debug(fmt,args...)  printf (fmt ,##args)
#else
  #define debug(fmt,args...)
#endif

/* To caculate the task array lenth */
#define ARRAY_LEN(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))

/* common status return code */
#define    GEN_NO_ERR                     0
#define    GEN_INIT_ERR                  -1
#define    GEN_RUN_ERR                   -2
#define    GEN_NULL_P_ERR                -3
#define    GEN_IO_ERR                    -4
#define    GEN_MEM_ERR                   -5
#define    GEN_DEINITIALIZED             -6
#define    GEN_INVALID_P                 -9

/* NULL Pointer Dereference Guard */
#define _RETURN_IF_FAIL(p)         \
  do{                    \
    if ((p) == NULL )              \
    {                  \
       debug(      \
       "%s:%d [W] Null pointer use in function: %s!\n",             \
       __FILE__,   \
       __LINE__,   \
       __func__);  \
      return;          \
    }                  \
  }                    \
  while(0)

#define _RETURN_P_IF_FAIL(p)              \
  do{                    \
    if ((p) == NULL )              \
    {                  \
       debug("%s:%d [W] Null pointer use in function: %s!\n",             \
              __FILE__,   \
              __LINE__,   \
              __func__);  \
      return NULL;              \
    }                  \
  }                    \
  while(0)

#define _RETURN_VAL_IF_FAIL(p, val)        \
  do{                \
    if ( (p) == NULL )          \
    {          \
        debug(    \
        "%s:%d [W] Null pointer use in function: %s!\n", \
        __FILE__, \
        __LINE__, \
        __func__);\
        return (val);  \
      }              \
  }                \
  while(0)

/*
 *
 * \brief safe free macros to avoid wild pointer 
 * 
 * \note _SAFE_FREE_REF_POINTER can only be used with pointer which has
 *    a reference count
 *
 */

#define _SAFE_FREE_REF_POINTER(p)  \
  do {              \
    _RETURN_P_IF_FAIL((p));  \
    if((p)->_refcnt > 1){  \
        (p)->_refcnt--;  \
    }else{      \
        free((p));  \
              (p) = NULL;  \
                }    \
  }      \
  while(0)  

#define _SAFE_FREE(p)          \
  do {      \
        free((p));  \
          (p) = NULL;  \
  }      \
  while(0)  



/* function return status code */
typedef int16_t status_t;


#endif /*__COMMON_H*/

