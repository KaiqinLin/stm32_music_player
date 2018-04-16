#ifndef __PLAYER_H
#define __PLAYER_H
#include "./player/player.h"
#include "./scheduler/scheduler.h"
#include <inttypes.h>
#include "./malloc/malloc.h"

/* 处理立体声音频数据时，输出缓冲区需要的最大大小为2304*16/8字节(16为PCM数据为16位)，
 * 这里我们定义MP3BUFFER_SIZE为2304，实际输出缓冲区为MP3BUFFER_SIZE*2个字节
 */
#define MP3BUFFER_SIZE  2304
#define INPUTBUF_SIZE   2000


typedef  short  srt_buf_array[MP3BUFFER_SIZE];

typedef struct _player_ctx_t {
  uint8_t       *file_name;            /*<! the selected mp3 file name >*/
  uint8_t        ucvolume;             /*<! setting volume >*/
  uint8_t        ucstatus;             /*<! playing status >*/
  uint32_t       ucfreq;
  buf_t         *input_buf;            /*<! pointer to the buffer of read from file>*/
  srt_buf_array *output_buf;           /*<! pointer to the buffer decoded output >*/
} player_ctx_t;

extern player_ctx_t    play_ctx;
/* 状态 */
enum
{
  STA_IDLE = 0,  /* 待机状态 */
  STA_PLAYING,  /* 放音状态 */
  STA_ERR,      /*  error  */
};

typedef struct
{
  uint8_t ucVolume;      /* 当前放音音量 */
  uint8_t ucStatus;      /* 状态，0表示待机，1表示播放中，2 出错 */  
  uint32_t ucFreq;      /* 采样频率 */
}MP3_TYPE;  

void player_init(task_t *s, void *ctx);
void player_task(task_t *s, void *ctx);

#endif /* __PLAYER_H*/

