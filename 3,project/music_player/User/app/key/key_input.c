#include "./key/key_input.h"
#include "common.h"


key_input_ctx_t g_key_input_ctx = {
  .vol_up_falg = 0,
  .vol_down_flag = 0,
  .back_flag = 0,
  .menu_flag = 0,
  .right_flag = 0,
  .down_flag = 0,
  .mid_flag = 0,
  .up_flag = 0,
  .left_flag = 0
};


void key_input_task(task_t *s, void *ctx)
{
  key_input_ctx_t *pctx = (key_input_ctx_t *)ctx;
  pctx->vol_up_falg = 0;
  pctx->vol_down_flag = 0;
  pctx->back_flag = 0;
  pctx->menu_flag = 0;
  pctx->right_flag = 0;
  pctx->down_flag = 0;
  pctx->mid_flag = 0;
  pctx->up_flag = 0;
  pctx->left_flag = 0;

  if (KEY_VOLUME_UP_READ) {
    pctx->vol_up_falg = 1;
  }
  if (KEY_VOLUME_DOWN_READ) {
    pctx->vol_down_flag = 1;
  }
  if (KEY_BACK_READ) {
    pctx->back_flag = 1;
  }
  if (KEY_MENU_READ) {
    pctx->menu_flag = 1;
  }
  if (KEY_RIGHT_READ) {
    pctx->right_flag = 1;
  }
  if (KEY_DOWN_READ) {
    pctx->down_flag = 1;
  }
  if (KEY_MID_READ) {
    pctx->mid_flag = 1;
  }
  if (KEY_UP_READ) {
    pctx->up_flag = 1;
  }
  if (KEY_LEFT_READ) {
    pctx->left_flag = 1;
  }
}
