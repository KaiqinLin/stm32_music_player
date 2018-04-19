#ifndef __UI_H
#define __UI_H
#include "stm32f4xx.h"
#include "./scheduler/scheduler.h"

typedef enum {
  PLAYING = 0x00,
  MENU
} current_win_t;

typedef struct _ui_ctx_t {
  current_win_t  current_win;
} ui_ctx_t;

extern ui_ctx_t   ui_ctx;

void gui_task(task_t *s, void *ctx);

#endif /* __UI_H*/

