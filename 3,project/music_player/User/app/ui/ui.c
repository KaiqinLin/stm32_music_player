#include "./ui/ui.h"
#include "GUI.h"
#include "common.h"
#include "./key/key_input.h"
#include "file_view.h"
#include "play_view.h"
#include "ff.h"

WM_HWIN g_page[2];

ui_ctx_t g_ui_ctx = {
   .current_win = PLAYING,
};

void sys_gui_init(task_t *s, void *ctx)
{
  ff_refresh_music_file(g_music_content);

  for (uint8_t i = 0; i < MAX_LIST_LEN; i++) {
     debug("%s\r\n", g_music_content[i]);
  }

  g_page[0] = Createplay_view();
  g_page[1] = Createfile_view();
  WM_HideWindow(g_page[1]);
  WM_ShowWindow(g_page[0]);

  
}


void gui_task(task_t *s, void *ctx)
{
  ui_ctx_t *pctx = (ui_ctx_t*) ctx;
  _RETURN_IF_FAIL(pctx);

  if (pctx->current_win == MENU) {
     if (g_key_input_ctx.up_flag == 1) {
       //TODO Select the object
     } else if (g_key_input_ctx.down_flag == 1) {
       //TODO Select the object
     } else if (g_key_input_ctx.mid_flag == 1) {
       //TODO Play the object
     } else if (g_key_input_ctx.left_flag == 1) {
       //TODO Refresh the list
     } else if (g_key_input_ctx.right_flag == 1) {
       //TODO Refresh the list
     } else if (g_key_input_ctx.back_flag == 1) {
       pctx->current_win = PLAYING;
       WM_HideWindow(g_page[1]);
       WM_ShowWindow(g_page[0]);
       //TODO Back to the playing window
     }
     if (g_key_input_ctx.vol_up_falg == 1) {
       //TODO Set the volume up
     } else if (g_key_input_ctx.vol_down_flag == 1) {
       //TODO Set the volume down
     }
  } else if (pctx->current_win == PLAYING) {
     //TODO Update the window lyric and bar
     if (g_key_input_ctx.up_flag == 1) {
       //TODO Fast foward
     } else if (g_key_input_ctx.down_flag == 1) {
       //TODO Fast backward
     } else if (g_key_input_ctx.left_flag == 1) {
       //TODO Select the last object and play
     } else if (g_key_input_ctx.right_flag == 1) {
       //TODO Select the next object and play
     }
     if (g_key_input_ctx.vol_up_falg == 1) {
       //TODO Set the volume up and update the window
     } else if (g_key_input_ctx.vol_down_flag == 1) {
       //TODO Set the volume down and update the window
     }
     if (g_key_input_ctx.menu_flag == 1) {
       //TODO Switch window
       WM_HideWindow(g_page[0]);
       WM_ShowWindow(g_page[1]);
       pctx->current_win = MENU;
     }
  }
  GUI_Exec();
  /* Reset the flag */
  g_key_input_ctx.vol_up_falg = 0;
  g_key_input_ctx.vol_down_flag = 0;
  g_key_input_ctx.back_flag = 0;
  g_key_input_ctx.menu_flag = 0;
  g_key_input_ctx.right_flag = 0;
  g_key_input_ctx.down_flag = 0;
  g_key_input_ctx.mid_flag = 0;
  g_key_input_ctx.up_flag = 0;
  g_key_input_ctx.left_flag = 0;
}

