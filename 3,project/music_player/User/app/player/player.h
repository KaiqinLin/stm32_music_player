#ifndef __PLAYER_H
#define __PLAYER_H
#include "./player/player.h"
#include "./scheduler/scheduler.h"
#include <inttypes.h>
#include "./malloc/malloc.h"
#include "ff.h"

/* 处理立体声音频数据时，输出缓冲区需要的最大大小为2304*16/8字节(16为PCM数据为16位)，
 * 这里我们定义MP3BUFFER_SIZE为2304，实际输出缓冲区为MP3BUFFER_SIZE*2个字节
 */
#define MP3BUFFER_SIZE  2304
#define INPUTBUF_SIZE   2000
#define AUDIO_BUFFER_SIZE   1024*8

#define MUSIC_SW_EN     0x01
#define MUSIC_SW_DIS    0x00

typedef  short  srt_buf_array[AUDIO_BUFFER_SIZE];
typedef enum {
  MP3_FILE = 0x01,
  WAV_FILE
} audio_type_t;

typedef struct _player_ctx_t {
  uint8_t        file_name[60];            /*<! the selected mp3 file name >*/
  uint8_t        ucvolume;             /*<! setting volume >*/
  uint8_t        ucstatus;             /*<! playing status >*/
  uint32_t       ucfreq;
  __IO uint8_t   file_sw;
  buf_t         *input_buf;            /*<! pointer to the buffer of read from file>*/
  srt_buf_array *output_buf;           /*<! pointer to the buffer decoded output >*/
  uint8_t       *tbuf;
  audio_type_t   audio_file_type;
  FIL           *file;
} player_ctx_t;

extern player_ctx_t    g_play_ctx;
/* 状态 */
enum
{
  STA_IDLE = 0,  /* 待机状态 */
  STA_PLAYING,  /* 放音状态 */
  STA_SW,
  STA_PAUSE,
  STA_ERR,      /*  error  */
};


void player_init(task_t *s, void *ctx);
void player_task(task_t *s, void *ctx);

#endif /* __PLAYER_H*/

