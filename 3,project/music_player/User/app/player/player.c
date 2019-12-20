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


player_ctx_t    g_play_ctx =
{
  .ucvolume = 20,
  .ucstatus = STA_IDLE,
  .ucfreq   = I2S_AudioFreq_Default,
  .file_sw  = MUSIC_SW_DIS
};

HMP3Decoder     Mp3_Decoder;      /* mp3解码器指针  */
MP3FrameInfo    Mp3_FrameInfo;    /* mP3帧信息  */
FIL fmusic;                        /* 目标文件 */
__wavctrl wav_ctrl;

srt_buf_array outbuf[2];                /* 解码输出缓冲区，也是I2S输入数据，实际占用字节数：RECBUFFER_SIZE*2 */
uint8_t       tbuf[AUDIO_BUFFER_SIZE];

FRESULT result; 
UINT bw;                      /* File R/W count */

const uint16_t I2S_PSC_TBL[][5]=
{
    {800 ,256,5,12,1},        //8Khz采样率
    {1102,429,4,19,0},        //11.025Khz采样率 
    {1600,213,2,13,0},        //16Khz采样率
    {2205,429,4, 9,1},        //22.05Khz采样率
    {3200,213,2, 6,1},        //32Khz采样率
    {4410,271,2, 6,0},        //44.1Khz采样率
    {4800,258,3, 3,1},        //48Khz采样率
    {8820,316,2, 3,1},        //88.2Khz采样率
    {9600,344,2, 3,1},      //96Khz采样率
    {17640,361,2,2,0},      //176.4Khz采样率 
    {19200,393,2,2,0},      //192Khz采样率
};

uint8_t I2S2_SampleRate_Set(uint32_t samplerate)
{ 
    uint8_t  i = 0; 
    uint32_t tempreg = 0;
    samplerate /= 10;//缩小10倍   
    
    for (i = 0; i < (sizeof(I2S_PSC_TBL) / 10); i++)//看看改采样率是否可以支持
    {
        if (samplerate == I2S_PSC_TBL[i][0] ) break;
    }

    RCC_PLLI2SCmd(DISABLE);//先关闭PLLI2S
    if (i == (sizeof(I2S_PSC_TBL) / 10)) return 1;//搜遍了也找不到

    RCC_PLLI2SConfig((uint32_t)I2S_PSC_TBL[i][1], (uint32_t)I2S_PSC_TBL[i][2]);//设置I2SxCLK的频率(x=2)  设置PLLI2SN PLLI2SR
 
    RCC->CR |= 1 << 26;                    //开启I2S时钟
    while ((RCC->CR & 1<<27) == 0);        //等待I2S时钟开启成功. 
    tempreg  = I2S_PSC_TBL[i][3] << 0;    //设置I2SDIV
    tempreg |= I2S_PSC_TBL[i][4] << 8;    //设置ODD位
    tempreg |= 1 << 9;                    //使能MCKOE位,输出MCK
    SPI3->I2SPR = tempreg;            //设置I2SPR寄存器 
    return 0;
}

void wav_get_curtime(FIL*fx, __wavctrl *wavx)
{
    long long fpos;      
    wavx->totsec = wavx->datasize/(wavx->bitrate/8);    //歌曲总长度(单位:秒) 
    fpos = fx->fptr-wavx->datastart;                     //得到当前文件播放到的地方 
    wavx->cursec = fpos*wavx->totsec/wavx->datasize;    //当前播放到第多少秒了?    
}
void mp3_get_curtime(FIL *fx, MP3FrameInfo *mp3frminfo, audio_info_t *audioinfo)
{
    long long fpos;
    audioinfo->all_sec = fx->obj.objsize / (mp3frminfo->bitrate / 8);
    fpos = fx->fptr;
    audioinfo->cur_sec = fpos * audioinfo->all_sec / fx->obj.objsize;
}

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
  pctx->outpath = EAR_LEFT_ON | EAR_RIGHT_ON;
  wm8978_CfgAudioPath(DAC_ON, pctx->outpath);

  /* 调节音量，左右相同音量 */
  wm8978_SetOUT1Volume(pctx->ucvolume);

  /* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
  wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
  
  /*  初始化并配置I2S  */
  I2S_Stop();
  I2S_GPIO_Config();
  I2Sx_Mode_Config(I2S_Standard_Phillips, I2S_DataFormat_16b, pctx->ucfreq);  
  I2S_DMA_TX_Callback=MP3Player_I2S_DMA_TX_Callback;
//  I2Sx_TX_DMA_Init((uint16_t *)(pctx->output_buf)[0], (uint16_t *)(pctx->output_buf)[1], MP3BUFFER_SIZE);

  pctx->tbuf = tbuf;
  pctx->file = &fmusic;
  pctx->mp3decoder = Mp3_Decoder;
  pctx->mp3frameinfo = &Mp3_FrameInfo;
  pctx->wavctrl = &wav_ctrl;

  pctx->transferedflag = 0;

}

//WAV解析初始化
//fname:文件路径+文件名
//wavx:wav 信息存放结构体指针
//返回值:0,成功;1,打开文件失败;2,非WAV文件;3,DATA区域未找到.
uint8_t wav_decode_init(uint8_t* fname,__wavctrl* wavx)
{
    uint8_t  buf[512]; 
    uint32_t br=0;
    uint8_t  res=0;
    
    ChunkRIFF *riff;
    ChunkFMT *fmt;
    ChunkFACT *fact;
    ChunkDATA *data;
    res = f_open(&fmusic, (TCHAR*)fname,FA_READ);//打开文件
    if(res ==FR_OK)
    {
        f_read(&fmusic, buf, 512, &br);    //读取512字节在数据
        riff = (ChunkRIFF *)buf;        //获取RIFF块
        if (riff->Format==0X45564157)//是WAV文件
        {
            fmt=(ChunkFMT *)(buf+12);    //获取FMT块 
            fact=(ChunkFACT *)(buf+12+8+fmt->ChunkSize);//读取FACT块
            if(fact->ChunkID==0X74636166||fact->ChunkID==0X5453494C)wavx->datastart=12+8+fmt->ChunkSize+8+fact->ChunkSize;//具有fact/LIST块的时候(未测试)
            else wavx->datastart=12+8+fmt->ChunkSize;  
            data=(ChunkDATA *)(buf+wavx->datastart);    //读取DATA块
            if(data->ChunkID==0X61746164)//解析成功!
            {
                wavx->audioformat=fmt->AudioFormat;        //音频格式
                wavx->nchannels=fmt->NumOfChannels;        //通道数
                wavx->samplerate=fmt->SampleRate;        //采样率
                wavx->bitrate=fmt->ByteRate*8;            //得到位速
                wavx->blockalign=fmt->BlockAlign;        //块对齐
                wavx->bps=fmt->BitsPerSample;            //位数,16/24/32位
                
                wavx->datasize=data->ChunkSize;            //数据块大小
                wavx->datastart=wavx->datastart+8;        //数据流开始的地方. 
                 
                printf("wavx->audioformat:%d\r\n",wavx->audioformat);
                printf("wavx->nchannels:%d\r\n",wavx->nchannels);
                printf("wavx->samplerate:%d\r\n",wavx->samplerate);
                printf("wavx->bitrate:%d\r\n",wavx->bitrate);
                printf("wavx->blockalign:%d\r\n",wavx->blockalign);
                printf("wavx->bps:%d\r\n",wavx->bps);
                printf("wavx->datasize:%d\r\n",wavx->datasize);
                printf("wavx->datastart:%d\r\n",wavx->datastart);  
            } else res=3;//data区域未找到.
        } else res=2;//非wav文件
        
    } else res=1;//打开文件错误

    f_close(&fmusic);
    return 0;
}
uint32_t buffill(uint8_t *buf,uint16_t size,uint8_t bits)
{
    uint16_t readlen=0;
    uint32_t bread;
    uint16_t i;
    uint8_t *p;
    if(bits==24)//24bit音频,需要处理一下
    {
        readlen=(size/4)*3;                            //此次要读取的字节数
        f_read(g_play_ctx.file,g_play_ctx.tbuf,readlen,(UINT*)&bread);    //读取数据
        p=g_play_ctx.tbuf;
        for(i=0;i<size;)
        {
            buf[i++]=p[1];
            buf[i]=p[2]; 
            i+=2;
            buf[i++]=p[0];
            p+=3;
        } 
        bread=(bread*4)/3;        //填充后的大小.
    }else 
    {
        f_read(g_play_ctx.file,buf,size,(UINT*)&bread);//16bit音频,直接读取数据  
        if(bread<size)//不够数据了,补充0
        {
            for(i=bread;i<size-bread;i++)buf[i]=0; 
        }
    }
    return bread;
}

void player_task(task_t *s, void *ctx)
{
  player_ctx_t *pctx = (player_ctx_t *)ctx;
  _RETURN_IF_FAIL(pctx);
  uint8_t res;
  static uint32_t fillnum;
  /* MP3 availables */
  static uint8_t *read_ptr;
  static int  err = 0, i = 0, outputSamps = 0;  
  static int  read_offset = 0;        /* 读偏移指针 */
  static int  bytes_left = 0;          /* 剩余字节数 */ 

  if (pctx->ucstatus == STA_SW) {
  /* Open a new music file */
    f_close(pctx->file);
    I2S_Stop();
    if (pctx->audio_file_type == WAV_FILE) {
    /* file type is wav */
      res = wav_decode_init(pctx->file_name, pctx->wavctrl);
      /* set file decode info */
      g_ui_ctx.audio_info.bitrate  = pctx->wavctrl->bitrate / 1000;
      g_ui_ctx.audio_info.samprate = pctx->wavctrl->samplerate;
      g_ui_ctx.audio_info.channels = pctx->wavctrl->nchannels;
      g_ui_ctx.info_upd_flag = 1;
      g_ui_ctx.audio_info.filename = pctx->file_name;
      /* update the cur sec */
      wav_get_curtime(pctx->file, pctx->wavctrl);
      g_ui_ctx.audio_info.all_sec = pctx->wavctrl->totsec;
      g_ui_ctx.audio_info.cur_sec = pctx->wavctrl->cursec;

      if (res == 0) {
        if (pctx->wavctrl->bps == 16) {
          wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
          I2Sx_Mode_Config(I2S_Standard_Phillips, I2S_DataFormat_16bextended, I2S_AudioFreq_Default);
        } else if (pctx->wavctrl->bps == 24) {
          wm8978_CfgAudioIF(I2S_Standard_Phillips, 24);
          I2Sx_Mode_Config(I2S_Standard_Phillips, I2S_DataFormat_24b, I2S_AudioFreq_Default);        //飞利浦标准,主机发送,时钟低电平有效,24位扩展帧长度
        }
        I2S2_SampleRate_Set(pctx->wavctrl->samplerate);             //设置采样率
        I2Sx_TX_DMA_Init((uint16_t *)pctx->output_buf[0], (uint16_t *)pctx->output_buf[1],AUDIO_BUFFER_SIZE/2); //配置TX DMA
        I2S_Stop();
        res = f_open(pctx->file, (TCHAR *)pctx->file_name, FA_READ);    //打开文件
        if (res == 0) {
          f_lseek(pctx->file, pctx->wavctrl->datastart);             //跳过文件头
          fillnum = buffill((uint8_t *)pctx->output_buf[0], AUDIO_BUFFER_SIZE, pctx->wavctrl->bps);
          fillnum = buffill((uint8_t *)pctx->output_buf[1], AUDIO_BUFFER_SIZE, pctx->wavctrl->bps);
          I2S_Play_Start();
        }
      }
    } else if (pctx->audio_file_type == MP3_FILE) {
      /* file type is mp3 */
      MP3FreeDecoder(pctx->mp3decoder);

      result = f_open(pctx->file, (char *)pctx->file_name, FA_READ);

      if (result != FR_OK) {
        debug("Open mp3file :%s fail!!!->%d\r\n", pctx->file_name, result);
        result = f_close (pctx->file);
        return;  /* 停止播放 */
      }
      debug("当前播放文件 -> %s\n", pctx->file_name);

      //初始化MP3解码器
      pctx->mp3decoder = MP3InitDecoder();  
      if(pctx->mp3decoder==0)
      {
        debug("初始化helix解码库设备\n");
        return;  /* 停止播放 */
      }
      debug("初始化中...\n");
      
      pctx->ucstatus = STA_PLAYING;
      result=f_read(pctx->file, pctx->input_buf->_base, INPUTBUF_SIZE, &bw);
      if(result!=FR_OK)
      {
        debug("读取%s失败 -> %d\r\n", pctx->file_name, result);
        MP3FreeDecoder(pctx->mp3decoder);
        return;
      }

      I2Sx_Mode_Config(I2S_Standard_Phillips, I2S_DataFormat_16b, pctx->ucfreq);
      read_ptr = pctx->input_buf->_base;
      bytes_left=bw;
      pctx->transferedflag = 1;
      pctx->ucfreq = I2S_AudioFreq_Default;
    }
    pctx->ucstatus = STA_PLAYING;

  } else if (pctx->ucstatus == STA_PLAYING) {
    if (pctx->transferedflag == 0) {
      /* DMA transmit not completed do nothing */
    } else {
      pctx->transferedflag = 0;

      if (pctx->audio_file_type == WAV_FILE) {
        if (fillnum != AUDIO_BUFFER_SIZE) {
          f_close(pctx->file);
          pctx->ucstatus = STA_IDLE;
          I2S_Stop();

          pctx->ucstatus = STA_NEXT;
        }
        if (pctx->bufferflag) {
          fillnum = buffill((uint8_t *)pctx->output_buf[1], AUDIO_BUFFER_SIZE, pctx->wavctrl->bps);//填充buf2
        } else {
          fillnum = buffill((uint8_t *)pctx->output_buf[0], AUDIO_BUFFER_SIZE, pctx->wavctrl->bps);//填充buf1
        }
        I2S_Play_Start();
        /* update the cur sec */
        wav_get_curtime(pctx->file, pctx->wavctrl);
        g_ui_ctx.audio_info.cur_sec = pctx->wavctrl->cursec;
        
      } else if (pctx->audio_file_type == MP3_FILE) {

        //寻找帧同步，返回第一个同步字的位置
        read_offset = MP3FindSyncWord(read_ptr, bytes_left);
        //没有找到同步字
        while (read_offset < 0) {
          // TODO :ADDED the timer
          result = f_read(pctx->file, pctx->input_buf->_base, INPUTBUF_SIZE, &bw);
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
          result = f_read(pctx->file, pctx->input_buf->_base + bytes_left + i, INPUTBUF_SIZE - bytes_left - i, &bw);
          bytes_left += bw;                    //有效数据流大小
        }
        //开始解码 参数：mp3解码结构体、输入流指针、输入流大小、输出流指针、数据格式
        err = MP3Decode(pctx->mp3decoder, &read_ptr, &bytes_left, pctx->output_buf[pctx->bufferflag], 0);          

      //错误处理
        if (err != ERR_MP3_NONE)                  
        {
          switch (err)
          {
            case ERR_MP3_INDATA_UNDERFLOW:
              debug("ERR_MP3_INDATA_UNDERFLOW\r\n");
              result = f_read(pctx->file, pctx->input_buf->_base, INPUTBUF_SIZE, &bw);
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
          pctx->transferedflag = 1;
        }
        else    //解码无错误，准备把数据输出到PCM
        {
          MP3GetLastFrameInfo(pctx->mp3decoder, pctx->mp3frameinfo);    //获取解码信息        
          /* 输出到DAC */
          outputSamps = pctx->mp3frameinfo->outputSamps;              //PCM数据个数
          if (outputSamps > 0)
          {
            if (pctx->mp3frameinfo->nChans == 1)  //单声道
            {
              //单声道数据需要复制一份到另一个声道
              for (i = outputSamps - 1; i >= 0; i--)
              {
                pctx->output_buf[pctx->bufferflag][i * 2] = pctx->output_buf[pctx->bufferflag][i];
                pctx->output_buf[pctx->bufferflag][i * 2 + 1] = pctx->output_buf[pctx->bufferflag][i];
              }
              outputSamps *= 2;
            }//if (Mp3FrameInfo.nChans == 1)  //单声道
          }//if (outputSamps > 0)
        
          /* 根据解码信息设置采样率 */
          if (pctx->mp3frameinfo->samprate != pctx->ucfreq)  //采样率 
          {
            pctx->ucfreq = pctx->mp3frameinfo->samprate;

            debug(" \r\n Bitrate       %dKbps", pctx->mp3frameinfo->bitrate/1000);
            debug(" \r\n Samprate      %dHz", pctx->ucfreq);
            debug(" \r\n BitsPerSample %db", pctx->mp3frameinfo->bitsPerSample);
            debug(" \r\n nChans        %d", pctx->mp3frameinfo->nChans);
            debug(" \r\n Layer         %d", pctx->mp3frameinfo->layer);
            debug(" \r\n Version       %d", pctx->mp3frameinfo->version);
            debug(" \r\n OutputSamps   %d", pctx->mp3frameinfo->outputSamps);
            debug("\r\n");

            /* set file decode info */
            g_ui_ctx.audio_info.bitrate  = pctx->mp3frameinfo->bitrate / 1000;
            g_ui_ctx.audio_info.samprate = pctx->mp3frameinfo->samprate;
            g_ui_ctx.audio_info.channels = pctx->mp3frameinfo->nChans;
            g_ui_ctx.audio_info.filename = pctx->file_name;
            g_ui_ctx.info_upd_flag = 1;
            /* get mp3 file cur time */
            mp3_get_curtime(pctx->file, pctx->mp3frameinfo, &g_ui_ctx.audio_info);

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
      
        mp3_get_curtime(pctx->file, pctx->mp3frameinfo, &g_ui_ctx.audio_info);
        if(pctx->file->fptr == pctx->file->obj.objsize)     //mp3文件读取完成，退出
        {
          debug("END\r\n");
          f_close(pctx->file);
          pctx->ucstatus = STA_IDLE;
          I2S_Stop();

          pctx->ucstatus = STA_NEXT;
        }
      }
    }
  } else if (pctx->ucstatus == STA_NEXT) {
    if (g_ui_ctx.current_sel == g_music_process.list_len) {
      g_ui_ctx.current_sel = 0;
    } else {
      g_ui_ctx.current_sel ++;
    }
    sprintf((char *)pctx->file_name,"0:/%s",g_music_process.music_content[g_ui_ctx.current_sel]);
    /* Enable the file switch flag to switch play item */
    if (CheckSuffix((uint8_t *)g_play_ctx.file_name, (uint8_t *)".mp3", 4) == TRUE) {
      pctx->audio_file_type = MP3_FILE;
    } else {
      pctx->audio_file_type = WAV_FILE;
    }
    pctx->ucstatus = STA_SW;
  } else if (pctx->ucstatus == STA_PAUSE) {
    I2S_Stop();
    pctx->transferedflag = 1;
    pctx->ucfreq = I2S_AudioFreq_Default;
    /* for update the prog and time */
    if (pctx->audio_file_type == MP3_FILE) {
      mp3_get_curtime(pctx->file, pctx->mp3frameinfo, &g_ui_ctx.audio_info);
    } else if (pctx->audio_file_type == WAV_FILE) {
      wav_get_curtime(pctx->file, pctx->wavctrl);
      g_ui_ctx.audio_info.cur_sec = pctx->wavctrl->cursec;
    }
  }
}
/* DMA发送完成中断回调函数 */
/* 缓冲区内容已经播放完成，需要切换缓冲区，进行新缓冲区内容播放 
   同时读取WAV文件数据填充到已播缓冲区  */
void MP3Player_I2S_DMA_TX_Callback(void)
{
    if (I2Sx_TX_DMA_STREAM->CR & (1 << 19)) {
        g_play_ctx.bufferflag = 0;
    } else {
        g_play_ctx.bufferflag = 1;
    }
    g_play_ctx.transferedflag = 1;
}



