#ifndef __FILE_PROCESS_H
#define __FILE_PROCESS_H
#include "stm32f4xx.h"

#define  MAX_LIST_LEN         16
#define  MAX_FILE_NAME_LEN    50

typedef char _music_file_name_t[MAX_FILE_NAME_LEN];

extern _music_file_name_t g_music_content[MAX_LIST_LEN];

void ff_refresh_music_file(_music_file_name_t *s);


#endif /* __FILE_PROCESS_H*/

