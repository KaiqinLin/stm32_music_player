#include "file_process.h"
#include "string.h"
#include "ff.h"

TCHAR  g_path[] = "/";
/* use to display the file list in the lcd */
_music_file_name_t g_music_content[MAX_LIST_LEN];

music_process_t g_music_process = {
  .music_content = g_music_content,
  .path = g_path
};

FRESULT ff_open_dir(music_process_t *s);
void ff_refresh_music_file(music_process_t *s);
bool CheckSuffix(uint8_t *Str,uint8_t *Suffix,uint8_t SuffixLen);
bool CheckMultiSuffix(uint8_t *Str,uint8_t *SuffixStr);


/**
 * @brief :Find the single file suffix
 */
bool CheckSuffix(uint8_t *Str, uint8_t *Suffix, uint8_t SuffixLen)
{
  uint8_t Len;
  uint8_t i;

  if(Suffix==NULL||(!Suffix[0])) return TRUE;
  Len=strlen((void *)Str);
  
  if(Len<=SuffixLen) return FALSE;
  for(i=SuffixLen;i;i--)
  {
    if(Str[Len-i]!=Suffix[SuffixLen-i]) return FALSE;
  }

  return TRUE;
}

/**
 * @brief :Find the multiple file suffix
 */
bool CheckMultiSuffix(uint8_t *Str, uint8_t *SuffixStr)
{
  uint8_t i, j, SuffixStrLen;

  if(SuffixStr==NULL||(!SuffixStr[0])) return TRUE;

  SuffixStrLen=strlen((void *)SuffixStr)+1;
  
  for(i=0,j=0;j<SuffixStrLen;j++)
  {
    if(SuffixStr[j]=='|')
    {
      if(CheckSuffix(Str,&SuffixStr[i],j-i)==TRUE) return TRUE;
      i=j+1;
    }
    else if(SuffixStr[j]==0)
    {
      return CheckSuffix(Str,&SuffixStr[i],j-i);
    }
  }    
  
  return FALSE;
}
FRESULT ff_open_dir(music_process_t *s)
{
  FRESULT res;
  res = f_opendir(&s->direct, s->path);
  if (res != FR_OK) {
    debug("%s : Open directory failed %02x",
          __func__, 
          res);
  }
  return res;
}
void ff_refresh_music_file(music_process_t *s)
{
  FRESULT res;
  UINT i = 0;
//  char *file_seperate;
  static FILINFO fno;

  res = ff_open_dir(s);
  if (res == FR_OK) {
    for (;i < MAX_LIST_LEN;) {
      res = f_readdir(&s->direct, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) {
        f_closedir(&s->direct);
        break;                                        /* Break on error or end of dir */
      }
      if (fno.fattrib & AM_DIR) {                    /* It is a directory */
      } else {                                       /* It is a file. */
        debug("%s/%s\r\n", s->path, fno.fname);
        if (CheckMultiSuffix((uint8_t *)fno.fname, (uint8_t *)".mp3|.wav") == TRUE && ((fno.fname[0] & 0x80) != 0x80)) {
          strcpy(s->music_content[i], fno.fname);
          s->list_len = i;
          i ++;
        }
      }
    }
  }
}
