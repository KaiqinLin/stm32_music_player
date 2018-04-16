#include "./malloc/malloc.h"
#include <string.h>

buf_man_t g_buf_man = {
    default_alloc_buffer,
    default_free_buffer,
};
buf_man_t *__gp_buf_man = &g_buf_man;

void *default_alloc_buffer(const uint16_t size)
{
    buf_t* new = (buf_t *)malloc(sizeof(buf_t));
    _RETURN_VAL_IF_FAIL(new, NULL); 

    new->_base = malloc(size);
    if( !new->_base )
    {
        debug(
               "[E]: %s: allocate buffer failed. no enough memory!\r\n",
           __func__);
    _SAFE_FREE(new);
    return NULL;
    }
    else
    {
        new->_size = size;
    return new;
    }
}

status_t default_free_buffer(buf_t *buf)
{
    _RETURN_VAL_IF_FAIL(buf, GEN_INVALID_P);
    memset(buf->_base, 0, buf->_size);  /* erase buffer */
    _SAFE_FREE(buf->_base);             /* return buffer memory to pool */
    _SAFE_FREE(buf);                    /* return metadata memory to pool */
    return GEN_NO_ERR;
}

