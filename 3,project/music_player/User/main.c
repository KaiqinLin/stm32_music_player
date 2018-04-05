#include "stm32f4xx.h"
#include "./usart/bsp_usart.h"
#include "./sdio/bsp_sdio.h"
#include "./test/sdio_test.h"
#include "./led/bsp_led.h"
#include "ff.h"
#include "./wm8978/bsp_wm8978.h"
#include "./systick/bsp_systick.h"
#include "./test/mp3Player.h"
//#include "./test/Recorder.h"
#include "./lcd/bsp_lcd.h"
#include "./test/lcd_test.h"


FATFS fs;

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
//  FRESULT result;
//    /* 程序来到main函数之前，启动文件：statup_stm32f429xx.s已经调用
//    * SystemInit()函数把系统时钟初始化成180MHZ
//    * SystemInit()在system_stm32f4xx.c中定义
//    * 如果用户想修改系统时钟，可自行编写程序修改
//    */
//      /* add your code here ^_^. */
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//    Debug_USART_Config();
//    LED_GPIO_Config();

//  /* 挂载SD卡文件系统 */
//  result = f_mount(&fs,"0:",1);  //挂载文件系统
//  if(result!=FR_OK)
//  {
//    printf("\n SD卡文件系统挂载失败\n");
//    while(1);
//  }
//  
//  /* 初始化系统滴答定时器 */
  SysTick_Init();  
//  printf("MP3播放器\n");
//  
//  /* 检测WM8978芯片，此函数会自动配置CPU的GPIO */
//  if (wm8978_Init()==0)
//  {
//    printf("检测不到WM8978芯片!!!\n");
//    while (1);  /* 停机 */
//  }
//  printf("初始化WM8978成功\n");
  lcd_test_case();
//	RecorderDemo();
  while(1)
  {    
//    mp3PlayerDemo("0:/谭咏麟 - 一生中最爱.mp3");
  }

}

/*********************************************END OF FILE**********************/

