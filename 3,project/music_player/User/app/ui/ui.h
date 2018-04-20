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

typedef struct _ui_ctx_t {
  current_win_t  current_win;
  uint16_t       current_sel;
} ui_ctx_t;

extern ui_ctx_t   g_ui_ctx;

void sys_gui_init(task_t *s, void *ctx);
void gui_task(task_t *s, void *ctx);

#endif /* __UI_H*/

