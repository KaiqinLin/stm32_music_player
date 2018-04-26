#ifndef __PLAYER_H
#define __PLAYER_H
#include "./player/player.h"
#include "./scheduler/scheduler.h"
#include <inttypes.h>
#include "./malloc/malloc.h"
#include "ff.h"
#include "mp3dec.h"
#include "file_process.h"
#include "./ui/ui.h"

/* 处理立体声音频数据时，输出缓冲区需要的最大大小为2304*16/8字节(16为PCM数据为16位)，
 * 这里我们定义MP3BUFFER_SIZE为2304，实际输出缓冲区为MP3BUFFER_SIZE*2个字节
 */
#define MP3BUFFER_SIZE  2304
#define INPUTBUF_SIZE   2000
#define AUDIO_BUFFER_SIZE   1024*4       /*!< the declare type is short type >*/

#define MUSIC_SW_EN     0x01
#define MUSIC_SW_DIS    0x00

//RIFF块
typedef struct
{
    uint32_t ChunkID;               //chunk id;这里固定为"RIFF",即0X46464952
    uint32_t ChunkSize ;               //集合大小;文件总大小-8
    uint32_t Format;                   //格式;WAVE,即0X45564157
}ChunkRIFF ;

//fmt块
typedef struct
{
    uint32_t ChunkID;               //chunk id;这里固定为"fmt ",即0X20746D66
    uint32_t ChunkSize ;               //子集合大小(不包括ID和Size);这里为:20.
    uint16_t AudioFormat;          //音频格式;0X01,表示线性PCM;0X11表示IMA ADPCM
    uint16_t NumOfChannels;        //通道数量;1,表示单声道;2,表示双声道;
    uint32_t SampleRate;            //采样率;0X1F40,表示8Khz
    uint32_t ByteRate;            //字节速率; 
    uint16_t BlockAlign;            //块对齐(字节); 
    uint16_t BitsPerSample;        //单个采样数据大小;4位ADPCM,设置为4
//    uint16_t ByteExtraData;        //附加的数据字节;2个; 线性PCM,没有这个参数
}ChunkFMT;

//fact块 
typedef struct 
{
    uint32_t ChunkID;               //chunk id;这里固定为"fact",即0X74636166;
    uint32_t ChunkSize ;               //子集合大小(不包括ID和Size);这里为:4.
    uint32_t NumOfSamples;          //采样的数量; 
}ChunkFACT;

//LIST块 
typedef struct 
{
    uint32_t ChunkID;               //chunk id;这里固定为"LIST",即0X74636166;
    uint32_t ChunkSize ;               //子集合大小(不包括ID和Size);这里为:4. 
}ChunkLIST;

//data块 
typedef struct 
{
    uint32_t ChunkID;               //chunk id;这里固定为"data",即0X5453494C
    uint32_t ChunkSize ;               //子集合大小(不包括ID和Size) 
}ChunkDATA;

//wav头
typedef struct
{ 
    ChunkRIFF riff;    //riff块
    ChunkFMT fmt;      //fmt块
//    ChunkFACT fact;    //fact块 线性PCM,没有这个结构体     
    ChunkDATA data;    //data块         
}__WaveHeader; 

//wav 播放控制结构体
typedef struct
{ 
    uint16_t audioformat;            //音频格式;0X01,表示线性PCM;0X11表示IMA ADPCM
    uint16_t nchannels;                //通道数量;1,表示单声道;2,表示双声道; 
    uint16_t blockalign;                //块对齐(字节);  
    uint32_t datasize;                //WAV数据大小 

    uint32_t totsec ;                //整首歌时长,单位:秒
    uint32_t cursec ;                //当前播放时长
    
    uint32_t bitrate;                   //比特率(位速)
    uint32_t samplerate;                //采样率 
    uint16_t bps;                    //位数,比如16bit,24bit,32bit
    
    uint32_t datastart;                //数据帧开始的位置(在文件里面的偏移)
}__wavctrl; 

/* Audio buffer type */
typedef  short  srt_buf_array[AUDIO_BUFFER_SIZE];

typedef enum {
  MP3_FILE = 0x01,     /*!< file type to select various decoder >*/
  WAV_FILE
} audio_type_t;
/**
 * @brief palyer ctx including all the information need
 */
typedef struct _player_ctx_t {
  uint8_t        file_name[60];            /*<! the selected mp3 file name >*/
  uint8_t        ucvolume;             /*<! setting volume >*/
  uint8_t        ucstatus;             /*<! playing status >*/
  uint32_t       ucfreq;
  __IO uint8_t   file_sw;
  buf_t         *input_buf;            /*<! pointer to the buffer of read from file>*/
  srt_buf_array *output_buf;           /*<! pointer to the buffer decoded output >*/
  uint8_t       *tbuf;
  audio_type_t   audio_file_type;
  FIL           *file;
  __wavctrl     *wavctrl;
  HMP3Decoder   *mp3decoder;
  MP3FrameInfo  *mp3frameinfo;
  __IO uint8_t   transferedflag;
  __IO uint8_t   bufferflag;
} player_ctx_t;

extern player_ctx_t    g_play_ctx;
/* 状态 */
enum
{
  STA_IDLE = 0,  /* 待机状态 */
  STA_PLAYING,  /* 放音状态 */
  STA_SW,
  STA_PAUSE,
  STA_NEXT,
  STA_ERR,      /*  error  */
};


void player_init(task_t *s, void *ctx);
void player_task(task_t *s, void *ctx);

#endif /* __PLAYER_H*/

