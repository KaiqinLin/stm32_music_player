#ifndef __FILE_PROCESS_H
#define __FILE_PROCESS_H
#include "stm32f4xx.h"
#include "ff.h"

#define  MAX_LIST_LEN         50
#define  MAX_FILE_NAME_LEN    40

typedef char _music_file_name_t[MAX_FILE_NAME_LEN];

typedef struct _music_process_t {
  _music_file_name_t  *music_content;            /*<! Pointer to the content buffer */
  DIR                  direct;                   /*<! To open the directory */
  TCHAR               *path;                     /*<! The path of directory >*/
  uint8_t              list_len;
} music_process_t;

extern music_process_t g_music_process;

FRESULT ff_open_dir(music_process_t *s);
void ff_refresh_music_file(music_process_t *s);


#endif /* __FILE_PROCESS_H*/

