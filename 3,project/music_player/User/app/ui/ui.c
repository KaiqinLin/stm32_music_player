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
audio_info_buffer_t audio_info_buffer;
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
  static uint32_t  timecnt = 0, last_timecnt = 0;
  static uint8_t   seek_state = 0, last_seek_state = 0;

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
       if (CheckSuffix((uint8_t *)g_play_ctx.file_name, (uint8_t *)".mp3", 4) == TRUE) {
         g_play_ctx.audio_file_type = MP3_FILE;
       } else {
         g_play_ctx.audio_file_type = WAV_FILE;
       }
       g_play_ctx.ucstatus = STA_SW;
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
       //TODO Fast backward
       g_play_ctx.ucstatus = STA_PAUSE;
       if (g_play_ctx.audio_file_type == MP3_FILE) {
         if (g_play_ctx.file->fptr > 30 * MP3BUFFER_SIZE) {
           f_lseek(g_play_ctx.file, f_tell(g_play_ctx.file) - 30 * MP3BUFFER_SIZE);
         }
       } else if (g_play_ctx.audio_file_type == WAV_FILE) {
         if (g_play_ctx.file->fptr > 60 * AUDIO_BUFFER_SIZE) {
           f_lseek(g_play_ctx.file, f_tell(g_play_ctx.file) - 60 * AUDIO_BUFFER_SIZE);
         }
       }
       seek_state = 1;
     } else if (g_key_input_ctx.down_flag == 1) {
       //TODO Fast forward
       g_play_ctx.ucstatus = STA_PAUSE;
       if (g_play_ctx.audio_file_type == MP3_FILE) {
         if (g_play_ctx.file->fptr < g_play_ctx.file->obj.objsize - 30 * MP3BUFFER_SIZE) {
           f_lseek(g_play_ctx.file, f_tell(g_play_ctx.file) + 30 * MP3BUFFER_SIZE);
         }
       } else if (g_play_ctx.audio_file_type == WAV_FILE) {
         if (g_play_ctx.file->fptr < g_play_ctx.file->obj.objsize - 60 * AUDIO_BUFFER_SIZE) {
           f_lseek(g_play_ctx.file, f_tell(g_play_ctx.file) + 60 * AUDIO_BUFFER_SIZE);
         }
       }
       seek_state = 1;

     } else if (g_key_input_ctx.left_flag == 1) {
       //TODO Select the last object and play
       if (pctx->current_sel != 0) {
         pctx->current_sel --;
       } else {
         pctx->current_sel = g_music_process.list_len;
       }
       sprintf((char *)g_play_ctx.file_name,"0:/%s",g_music_process.music_content[pctx->current_sel]);
       /* Enable the file switch flag to switch play item */
       if (CheckSuffix((uint8_t *)g_play_ctx.file_name, (uint8_t *)".mp3", 4) == TRUE) {
         g_play_ctx.audio_file_type = MP3_FILE;
       } else {
         g_play_ctx.audio_file_type = WAV_FILE;
       }
       g_play_ctx.ucstatus = STA_SW;
       debug("%s: Selcted: %s\r\n",
             __func__,
             g_play_ctx.file_name);

     } else if (g_key_input_ctx.right_flag == 1) {
       //TODO Select the next object and play
       if (pctx->current_sel == g_music_process.list_len) {
         pctx->current_sel = 0;
       } else {
         pctx->current_sel ++;
       }
       sprintf((char *)g_play_ctx.file_name,"0:/%s",g_music_process.music_content[pctx->current_sel]);
       /* Enable the file switch flag to switch play item */
       if (CheckSuffix((uint8_t *)g_play_ctx.file_name, (uint8_t *)".mp3", 4) == TRUE) {
         g_play_ctx.audio_file_type = MP3_FILE;
       } else {
         g_play_ctx.audio_file_type = WAV_FILE;
       }
       g_play_ctx.ucstatus = STA_SW;
       debug("%s: Selcted: %s\r\n",
             __func__,
             g_play_ctx.file_name);

     } else if (g_key_input_ctx.mid_flag == 1) {
         if (g_play_ctx.ucstatus == STA_PLAYING) {
           g_play_ctx.ucstatus = STA_PAUSE;
         } else if (g_play_ctx.ucstatus == STA_PAUSE) {
           g_play_ctx.ucstatus = STA_PLAYING;
         }
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

     /* restart the player */
     if ( last_seek_state == 1 && seek_state == 0) {
       last_seek_state = 0;
       g_play_ctx.ucstatus = STA_PLAYING;
     }


    timecnt = sched_get_micros(&g_sched);
    if (timecnt - last_timecnt >= 500) {
      last_timecnt = timecnt;
      /* Send refresh the player view msg */
      //TODO refresh the pro bar
      WM_MESSAGE msg;
      sprintf((char *)audio_info_buffer.cursecbuf, "%d:%02d", pctx->audio_info.cur_sec / 60, pctx->audio_info.cur_sec % 60);
      sprintf((char *)audio_info_buffer.allsecbuf, "%d:%02d", pctx->audio_info.all_sec / 60, pctx->audio_info.all_sec % 60);
      msg.MsgId = WM_REFRESH_PLAY_TIME;
      WM_SendMessage(WM_GetClientWindow(g_page[0]), &msg);

    } else {

    }
  }

  /* first play update the file information */
  if (pctx->info_upd_flag == 1) {
    WM_MESSAGE msg;
    sprintf((char *)audio_info_buffer.bitratebuf, "BITRATE: %d Bps", pctx->audio_info.bitrate);
    sprintf((char *)audio_info_buffer.sampratebuf, "SAMPLERATE: %.1f kHz", pctx->audio_info.samprate / 1000.f);
    sprintf((char *)audio_info_buffer.channelbuf, "CHANNELS: %d ", pctx->audio_info.channels);
    msg.MsgId = WM_REFRESH_PLAY_INFO;
    WM_SendMessage(WM_GetClientWindow(g_page[0]), &msg);
  }

  GUI_Exec();
  /* Reset the flag */
  last_seek_state = seek_state;
  seek_state = 0;
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

