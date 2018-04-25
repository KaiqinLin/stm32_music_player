#include "./player/music_player.h"
#include "./scheduler/scheduler.h"


uint8_t tbuf[AUDIO_BUFFER_SIZE];
__wavctrl wav_ctrl;
__IO uint8_t   audio_transfered = 0, audio_buf = 0;
FIL fmusic;

const u16 I2S_PSC_TBL[][5]=
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

u8 I2S2_SampleRate_Set(u32 samplerate)
{ 
    u8 i=0; 
    u32 tempreg=0;
    samplerate/=10;//缩小10倍   
    
    for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//看看改采样率是否可以支持
    {
        if(samplerate==I2S_PSC_TBL[i][0])break;
    }
 
    RCC_PLLI2SCmd(DISABLE);//先关闭PLLI2S
    if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//搜遍了也找不到
    RCC_PLLI2SConfig((u32)I2S_PSC_TBL[i][1],(u32)I2S_PSC_TBL[i][2]);//设置I2SxCLK的频率(x=2)  设置PLLI2SN PLLI2SR
 
    RCC->CR|=1<<26;                    //开启I2S时钟
    while((RCC->CR&1<<27)==0);        //等待I2S时钟开启成功. 
    tempreg=I2S_PSC_TBL[i][3]<<0;    //设置I2SDIV
    tempreg|=I2S_PSC_TBL[i][4]<<8;    //设置ODD位
    tempreg|=1<<9;                    //使能MCKOE位,输出MCK
    SPI3->I2SPR=tempreg;            //设置I2SPR寄存器 
    return 0;
}
void wav_get_curtime(FIL*fx,__wavctrl *wavx)
{
    long long fpos;      
     wavx->totsec=wavx->datasize/(wavx->bitrate/8);    //歌曲总长度(单位:秒) 
    fpos=fx->fptr-wavx->datastart;                     //得到当前文件播放到的地方 
    wavx->cursec=fpos*wavx->totsec/wavx->datasize;    //当前播放到第多少秒了?    
}

void music_player_init(task_t *s, void *ctx)
{
  player_ctx_t *pctx = (player_ctx_t *)ctx;
  _RETURN_IF_FAIL(pctx);

  pctx->tbuf = tbuf;
  pctx->file = &fmusic;


}

void player_i2s_dma_tx_callback(void) 
{   
    u16 i;
    if(I2Sx_TX_DMA_STREAM->CR&(1<<19)) {
        audio_buf = 0;
    } else {
        audio_buf = 1;
    }
    audio_transfered = 1;
}

//WAV解析初始化
//fname:文件路径+文件名
//wavx:wav 信息存放结构体指针
//返回值:0,成功;1,打开文件失败;2,非WAV文件;3,DATA区域未找到.
u8 wav_decode_init(u8* fname,__wavctrl* wavx)
{
    u8  buf[512]; 
    u32 br=0;
    u8  res=0;
    
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
            }else res=3;//data区域未找到.
        }else res=2;//非wav文件
        
    }else res=1;//打开文件错误

    f_close(&fmusic);
    return 0;
}
u32 buffill(u8 *buf,u16 size,u8 bits)
{
    u16 readlen=0;
    u32 bread;
    u16 i;
    u8 *p;
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
void music_player_task(task_t *s, void *ctx)
{
  player_ctx_t *pctx = (player_ctx_t *)ctx;
  _RETURN_IF_FAIL(pctx);
  uint8_t res;
  static uint32_t fillnum;

  if (pctx->ucstatus == STA_SW) {
    f_close(pctx->file);
    res = wav_decode_init(pctx->file_name, &wav_ctrl);
    if (res == 0) {
      if (wav_ctrl.bps == 16) {
        wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
        I2Sx_Mode_Config(I2S_Standard_Phillips, I2S_DataFormat_16bextended, I2S_AudioFreq_Default);
      } else if (wav_ctrl.bps == 24) {
        wm8978_CfgAudioIF(I2S_Standard_Phillips, 24);
        I2Sx_Mode_Config(I2S_Standard_Phillips, I2S_DataFormat_24b, I2S_AudioFreq_Default);        //飞利浦标准,主机发送,时钟低电平有效,24位扩展帧长度
      }
      I2S2_SampleRate_Set(wav_ctrl.samplerate);             //设置采样率
      I2Sx_TX_DMA_Init((uint16_t *)pctx->output_buf[0], (uint16_t *)pctx->output_buf[1],AUDIO_BUFFER_SIZE/2); //配置TX DMA
      I2S_DMA_TX_Callback = player_i2s_dma_tx_callback;           //回调函数指wav_i2s_dma_callback
      I2S_Stop();
      res = f_open(pctx->file, (uint8_t *)pctx->file_name, FA_READ);    //打开文件
      if (res == 0) {
        f_lseek(pctx->file, wav_ctrl.datastart);             //跳过文件头
        fillnum = buffill((uint8_t *)pctx->output_buf[0], AUDIO_BUFFER_SIZE, wav_ctrl.bps);
        fillnum = buffill((uint8_t *)pctx->output_buf[1], AUDIO_BUFFER_SIZE, wav_ctrl.bps);
        I2S_Play_Start();
      }
    }
    pctx->ucstatus = STA_PLAYING;
  } else if (pctx->ucstatus == STA_PLAYING) {
    if (audio_transfered == 0) {

    } else {
      audio_transfered = 0;
      if (fillnum != AUDIO_BUFFER_SIZE) {
        f_close(pctx->file);
        pctx->ucstatus = STA_IDLE;
        I2S_Stop();
      }
      if (audio_buf) {
        fillnum = buffill((uint8_t *)pctx->output_buf[1], AUDIO_BUFFER_SIZE, wav_ctrl.bps);//填充buf2
      } else {
        fillnum = buffill((uint8_t *)pctx->output_buf[0], AUDIO_BUFFER_SIZE, wav_ctrl.bps);//填充buf1
      }
    }

  }

}

