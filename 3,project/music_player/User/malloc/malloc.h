#ifndef _MALLOC_H
#define _MALLOC_H
#include "stm32f4xx.h"
#include "common.h"

/* general byte buffer */
typedef struct _buf {
    uint16_t   _size;
    uint8_t   *_base;
} buf_t;

typedef buf_t    *(* fp_alloc)(const uint16_t size);
typedef status_t  (* fp_free)(buf_t  *buf);

typedef struct _buf_man {
    fp_alloc   alloc_buffer;
    fp_free    free_buffer;
}buf_man_t;


void    *default_alloc_buffer(const uint16_t size);
status_t default_free_buffer(buf_t *buf);


extern buf_man_t *__gp_buf_man;

#endif


