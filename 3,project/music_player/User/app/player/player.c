#include <string.h>
#include "./player/player.h"
#include "./scheduler/scheduler.h"
#include "./usart/bsp_usart.h"
#include "./systick/bsp_systick.h"
#include "./wm8978/bsp_wm8978.h"
#include "common.h"
#include "ff.h" 
#include "mp3dec.h"
#include "./malloc/malloc.h"
/* 推荐使用以下格式mp3文件：
 * 采样率：44100Hz
 * 声  道：2
 * 比特率：320kbps
 */


player_ctx_t    g_play_ctx =
{
  .ucvolume = 20,
  .ucstatus = STA_IDLE,
  .ucfreq   = I2S_AudioFreq_Default,
  .file_sw  = MUSIC_SW_DIS
};
static HMP3Decoder     Mp3Decoder;      /* mp3解码器指针  */
static MP3FrameInfo    Mp3FrameInfo;    /* mP3帧信息  */
static __IO uint8_t bufflag = 0;             /* 数据缓存区选择标志 */
static __IO uint8_t is_read_flag = 0;        /*<! DMA transfer complete flag>*/

short outbuf[2][MP3BUFFER_SIZE];  /* 解码输出缓冲区，也是I2S输入数据，实际占用字节数：RECBUFFER_SIZE*2 */

FIL file;                      /* file objects */
FRESULT result; 
UINT bw;                      /* File R/W count */

/* 仅允许本文件内调用的函数声明 */
void MP3Player_I2S_DMA_TX_Callback(void);

void player_init(task_t *s, void *ctx)
{
  player_ctx_t *pctx = (player_ctx_t *)ctx;
  _RETURN_IF_FAIL(pctx);
  
  pctx->input_buf   = __gp_buf_man->alloc_buffer(INPUTBUF_SIZE);        /* 解码输入缓冲区，1940字节为最大MP3帧大小  */
  _RETURN_IF_FAIL(pctx->input_buf);
  pctx->output_buf = outbuf;

  Delay_ms(10);  /* 延迟一段时间，等待I2S中断结束 */
  wm8978_Reset();    /* 复位WM8978到复位状态 */

  /* 配置WM8978芯片，输入为DAC，输出为耳机 */
  wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

  /* 调节音量，左右相同音量 */
  wm8978_SetOUT1Volume(pctx->ucvolume);

  /* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
  wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
  
  /*  初始化并配置I2S  */
  I2S_Stop();
  I2S_GPIO_Config();
  I2Sx_Mode_Config(I2S_Standard_Phillips, I2S_DataFormat_16b, pctx->ucfreq);  
  I2S_DMA_TX_Callback=MP3Player_I2S_DMA_TX_Callback;
  I2Sx_TX_DMA_Init((uint16_t *)(pctx->output_buf)[0], (uint16_t *)(pctx->output_buf)[1], MP3BUFFER_SIZE);
  
  bufflag = 0;
  is_read_flag = 1;
}


void player_task(task_t *s, void *ctx)
{
  player_ctx_t *pctx = (player_ctx_t *)ctx;
  _RETURN_IF_FAIL(pctx);

  static uint8_t *read_ptr;
  static uint32_t frames = 0;
  static int  err = 0, i = 0, outputSamps = 0;  
  static int  read_offset = 0;        /* 读偏移指针 */
  static int  bytes_left = 0;          /* 剩余字节数 */  
//  read_ptr = pctx->input_buf->_base;

  if (pctx->file_sw == MUSIC_SW_EN) {
    f_close(&file);
    MP3FreeDecoder(Mp3Decoder);

    result = f_open(&file, (char *)pctx->file_name, FA_READ);

    if (result != FR_OK) {
      debug("Open mp3file :%s fail!!!->%d\r\n", pctx->file_name, result);
      result = f_close (&file);
      return;  /* 停止播放 */
    }
    debug("当前播放文件 -> %s\n", pctx->file_name);

    //初始化MP3解码器
    Mp3Decoder = MP3InitDecoder();  
    if(Mp3Decoder==0)
    {
      debug("初始化helix解码库设备\n");
      return;  /* 停止播放 */
    }
    debug("初始化中...\n");
    
    pctx->ucstatus = STA_PLAYING;
    result=f_read(&file, pctx->input_buf->_base, INPUTBUF_SIZE, &bw);
    if(result!=FR_OK)
    {
      debug("读取%s失败 -> %d\r\n", pctx->file_name, result);
      MP3FreeDecoder(Mp3Decoder);
      return;
    }

    read_ptr = pctx->input_buf->_base;
    bytes_left=bw;
  }

  if (pctx->ucstatus == STA_PLAYING)
  {
    pctx->file_sw = MUSIC_SW_DIS;
    if (is_read_flag == 1) {
      //寻找帧同步，返回第一个同步字的位置
      read_offset = MP3FindSyncWord(read_ptr, bytes_left);
      //没有找到同步字
      while (read_offset < 0) {
        // TODO :ADDED the timer
        result = f_read(&file, pctx->input_buf->_base, INPUTBUF_SIZE, &bw);
        read_offset = MP3FindSyncWord(read_ptr, bytes_left);
        if (result != FR_OK)
        {
          debug("读取%s失败 -> %d\r\n", pctx->file_name, result);
          return ;
        } else {
          read_ptr = pctx->input_buf->_base;
          bytes_left = bw;
          continue ;
        }
      }
      
      read_ptr += read_offset;          //偏移至同步字的位置
      bytes_left -= read_offset;        //同步字之后的数据大小  
      if (bytes_left < 1024)              //补充数据
      {
        /* 注意这个地方因为采用的是DMA读取，所以一定要4字节对齐  */
        i = (uint32_t)(bytes_left) & 3;                  //判断多余的字节
        if (i) i = 4 - i;                            //需要补充的字节
        memcpy(pctx->input_buf->_base + i, read_ptr, bytes_left);  //从对齐位置开始复制
        read_ptr = pctx->input_buf->_base + i;                    //指向数据对齐位置
        //补充数据
        result = f_read(&file, pctx->input_buf->_base + bytes_left + i, INPUTBUF_SIZE - bytes_left - i, &bw);
        bytes_left += bw;                    //有效数据流大小
      }
      //开始解码 参数：mp3解码结构体、输入流指针、输入流大小、输出流指针、数据格式
      err = MP3Decode(Mp3Decoder, &read_ptr, &bytes_left, pctx->output_buf[bufflag], 0);          
      frames++;  
      //错误处理
      if (err != ERR_MP3_NONE)                  
      {
        switch (err)
        {
          case ERR_MP3_INDATA_UNDERFLOW:
            debug("ERR_MP3_INDATA_UNDERFLOW\r\n");
            result = f_read(&file, pctx->input_buf->_base, INPUTBUF_SIZE, &bw);
            read_ptr = pctx->input_buf->_base;
            bytes_left = bw;
            break;    
          case ERR_MP3_MAINDATA_UNDERFLOW:
            /* do nothing - next call to decode will provide more mainData */
            debug("ERR_MP3_MAINDATA_UNDERFLOW\r\n");
            break;    
          default:
            debug("UNKNOWN ERROR:%d\r\n", err);    
            // 跳过此帧
            if (bytes_left > 0)
            {
              bytes_left --;
              read_ptr ++;
            }  
            break;
        }
        is_read_flag = 1;
      }
      else    //解码无错误，准备把数据输出到PCM
      {
        MP3GetLastFrameInfo(Mp3Decoder, &Mp3FrameInfo);    //获取解码信息        
        /* 输出到DAC */
        outputSamps = Mp3FrameInfo.outputSamps;              //PCM数据个数
        if (outputSamps > 0)
        {
          if (Mp3FrameInfo.nChans == 1)  //单声道
          {
            //单声道数据需要复制一份到另一个声道
            for (i = outputSamps - 1; i >= 0; i--)
            {
              pctx->output_buf[bufflag][i * 2] = pctx->output_buf[bufflag][i];
              pctx->output_buf[bufflag][i * 2 + 1] = pctx->output_buf[bufflag][i];
            }
            outputSamps *= 2;
          }//if (Mp3FrameInfo.nChans == 1)  //单声道
        }//if (outputSamps > 0)
        
        /* 根据解码信息设置采样率 */
        if (Mp3FrameInfo.samprate != pctx->ucfreq)  //采样率 
        {
          pctx->ucfreq = Mp3FrameInfo.samprate;
          
          debug(" \r\n Bitrate       %dKbps", Mp3FrameInfo.bitrate/1000);
          debug(" \r\n Samprate      %dHz", pctx->ucfreq);
          debug(" \r\n BitsPerSample %db", Mp3FrameInfo.bitsPerSample);
          debug(" \r\n nChans        %d", Mp3FrameInfo.nChans);
          debug(" \r\n Layer         %d", Mp3FrameInfo.layer);
          debug(" \r\n Version       %d", Mp3FrameInfo.version);
          debug(" \r\n OutputSamps   %d", Mp3FrameInfo.outputSamps);
          debug("\r\n");
          //I2S_AudioFreq_Default = 2，正常的帧，每次都要改速率
          if (pctx->ucfreq >= I2S_AudioFreq_Default)  
          {
            //根据采样率修改I2S速率
            I2Sx_Mode_Config(I2S_Standard_Phillips, I2S_DataFormat_16b, pctx->ucfreq);
            I2Sx_TX_DMA_Init((uint16_t *)pctx->output_buf[0],(uint16_t *)pctx->output_buf[1],outputSamps);
          }
          I2S_Play_Start();
        }
      }//else 解码正常
      
      if(file.fptr==file.obj.objsize)     //mp3文件读取完成，退出
      {
        debug("END\r\n");
        pctx->ucstatus = STA_IDLE;
        I2S_Stop();
      }
      is_read_flag = 0;
    } else {

    }
  }
}
/* DMA发送完成中断回调函数 */
/* 缓冲区内容已经播放完成，需要切换缓冲区，进行新缓冲区内容播放 
   同时读取WAV文件数据填充到已播缓冲区  */
void MP3Player_I2S_DMA_TX_Callback(void)
{
  if(I2Sx_TX_DMA_STREAM->CR & (1 << 19)) //当前使用Memory1数据
  {
    bufflag=0;                       //可以将数据读取到缓冲区0
  }
  else                               //当前使用Memory0数据
  {
    bufflag=1;                       //可以将数据读取到缓冲区1
  }
  is_read_flag = 1;                          // DMA传输完成标志
}



