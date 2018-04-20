#include "./ui/ui.h"
#include "./player/player.h"
#include "GUI.h"
#include "common.h"
#include "./key/key_input.h"
#include "file_view.h"
#include "play_view.h"
#include "ff.h"

WM_HWIN g_page[2];

ui_ctx_t g_ui_ctx = {
   .current_win = PLAYING,
   .current_sel = 0
};

void sys_gui_init(task_t *s, void *ctx)
{
  ff_refresh_music_file(&g_music_process);

  for (uint8_t i = 0; i < g_music_process.list_len; i++) {
     debug("%s\r\n", g_music_process.music_content[i]);
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
       /* Send key msg to the select obj(ListView) */
       GUI_SendKeyMsg(GUI_KEY_UP, 1);
     } else if (g_key_input_ctx.down_flag == 1) {
       //TODO Select the object
       /* Send key msg to the select obj(ListView) */
       GUI_SendKeyMsg(GUI_KEY_DOWN, 1);
     } else if (g_key_input_ctx.mid_flag == 1) {
       //TODO Play the object
       /* Get the selected row in ListView */
       pctx->current_sel = LISTVIEW_GetSel(WM_GetDialogItem(g_page[1], ID_LISTVIEW_0));
       sprintf((char *)g_play_ctx.file_name,"0:/%s",g_music_process.music_content[pctx->current_sel]);
       /* Enable the file switch flag to switch play item */
       g_play_ctx.file_sw = MUSIC_SW_EN;
       debug("%s: Selcted: %s\r\n",
             __func__,
             g_play_ctx.file_name);
     } else if (g_key_input_ctx.left_flag == 1) {
       //TODO Refresh the list
     } else if (g_key_input_ctx.right_flag == 1) {
       //TODO Refresh the list
     } else if (g_key_input_ctx.back_flag == 1) {
       WM_MESSAGE msg;
       msg.MsgId = WM_SELECT_WINDOW;
       /* Send user define message to the obj window to switch window */
       WM_SendMessage(WM_GetClientWindow(g_page[1]), &msg);
       pctx->current_win = PLAYING;
//       WM_HideWindow(g_page[1]);
//       WM_ShowWindow(g_page[0]);
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
       pctx->current_sel --;
       sprintf((char *)g_play_ctx.file_name,"0:/%s",g_music_process.music_content[pctx->current_sel]);
       /* Enable the file switch flag to switch play item */
       g_play_ctx.file_sw = MUSIC_SW_EN;
       debug("%s: Selcted: %s\r\n",
             __func__,
             g_play_ctx.file_name);

     } else if (g_key_input_ctx.right_flag == 1) {
       //TODO Select the next object and play
       pctx->current_sel ++;
       sprintf((char *)g_play_ctx.file_name,"0:/%s",g_music_process.music_content[pctx->current_sel]);
       /* Enable the file switch flag to switch play item */
       g_play_ctx.file_sw = MUSIC_SW_EN;
       debug("%s: Selcted: %s\r\n",
             __func__,
             g_play_ctx.file_name);

     }
     if (g_key_input_ctx.vol_up_falg == 1) {
       //TODO Set the volume up and update the window
     } else if (g_key_input_ctx.vol_down_flag == 1) {
       //TODO Set the volume down and update the window
     }
     if (g_key_input_ctx.menu_flag == 1) {
       //TODO Switch window
//       WM_HideWindow(g_page[0]);
//       WM_ShowWindow(g_page[1]);
//       WM_SetFocus(g_page[1]);
       WM_MESSAGE msg;
       msg.MsgId = WM_SELECT_WINDOW;
       WM_SendMessage(WM_GetClientWindow(g_page[0]), &msg);
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

