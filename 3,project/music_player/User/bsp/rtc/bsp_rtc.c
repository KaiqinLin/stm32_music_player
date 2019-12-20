#include "./rtc/bsp_rtc.h"


//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
  RTC_TimeTypeDef RTC_TimeTypeInitStructure;
  
  RTC_TimeTypeInitStructure.RTC_Hours=hour;
  RTC_TimeTypeInitStructure.RTC_Minutes=min;
  RTC_TimeTypeInitStructure.RTC_Seconds=sec;
  RTC_TimeTypeInitStructure.RTC_H12=ampm;
  
  return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
  
}
//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
  
  RTC_DateTypeDef RTC_DateTypeInitStructure;
  RTC_DateTypeInitStructure.RTC_Date = date;
  RTC_DateTypeInitStructure.RTC_Month = month;
  RTC_DateTypeInitStructure.RTC_WeekDay = week;
  RTC_DateTypeInitStructure.RTC_Year = year;
  return RTC_SetDate(RTC_Format_BIN, &RTC_DateTypeInitStructure);
}

//RTC初始化
//返回值:0,初始化成功;
//       1,LSE开启失败;
//       2,进入初始化模式失败;
uint8_t RTC_Config(void)
{
  RTC_InitTypeDef RTC_InitStructure;

  uint16_t retry = 0X1FFF; 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
  PWR_BackupAccessCmd(ENABLE);  //使能后备寄存器访问 
  
  if(RTC_ReadBackupRegister(RTC_BKP_DR1)!=0x5050)    //是否第一次配置?
  {
    RCC_LSEConfig(RCC_LSE_ON);//LSE 开启    
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)  //检查指定的RCC标志位设置与否,等待低速晶振就绪
    {
      retry++;
      Delay_ms(10);
    }
    if (retry == 0) {
      debug("%s: RTC 时钟初始化失败 \r\n", __func__);
      return 1;    //LSE 开启失败. 
    }
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);    //设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
    RCC_RTCCLKCmd(ENABLE);  //使能RTC时钟 

    RTC_InitStructure.RTC_AsynchPrediv = ASYNCHPREDIV;//RTC异步分频系数(1~0X7F)
    RTC_InitStructure.RTC_SynchPrediv  = SYNCHPREDIV;//RTC同步分频系数(0~7FFF)
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC设置为,24小时格式
    RTC_Init(&RTC_InitStructure);
 
    RTC_Set_Time(22, 28, 00, RTC_H12_PM);  //设置时间
    RTC_Set_Date(18, 5, 4, 5);    //设置日期
   
    RTC_WriteBackupRegister(RTC_BKP_DR1,0x5050);  //标记已经初始化过了
  }
  return 0;
}

