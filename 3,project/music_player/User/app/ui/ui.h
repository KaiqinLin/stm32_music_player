#ifndef __UI_H
#define __UI_H
#include "stm32f4xx.h"
#include "./scheduler/scheduler.h"
#include "file_process.h"

/**
 * @breif :Current display window
 */
typedef enum {
  PLAYING = 0x00,
  MENU
} current_win_t;

typedef struct _audio_info_t {
  uint8_t  *filename;
  uint32_t  samprate;
  uint32_t  bitrate;
  uint32_t  cur_sec;
  uint32_t  all_sec;
  uint8_t   channels;
} audio_info_t;

typedef struct _ui_ctx_t {
  current_win_t  current_win;
  uint16_t       current_sel;
  audio_info_t   audio_info;
  uint8_t        info_upd_flag;       /* 1 for update */
} ui_ctx_t;

typedef struct _audio_info_buffer_t {
  uint8_t   titlebuf[60];
  uint8_t   sampratebuf[30];
  uint8_t   bitratebuf[30];
  uint8_t   channelbuf[30];
  uint8_t   cursecbuf[10];
  uint8_t   allsecbuf[10];
  uint8_t   volbuf[8];
} audio_info_buffer_t;


extern audio_info_buffer_t audio_info_buffer;
extern ui_ctx_t   g_ui_ctx;

void sys_gui_init(task_t *s, void *ctx);
void gui_task(task_t *s, void *ctx);

#endif /* __UI_H*/

