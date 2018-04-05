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
#include "./malloc/malloc.h"
#include "GUI.h"


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
//    /* 程序来到main函数之前，启动文件：statup_stm32f429xx.s已经调用
//    * SystemInit()函数把系统时钟初始化成180MHZ
//    * SystemInit()在system_stm32f4xx.c中定义
//    * 如果用户想修改系统时钟，可自行编写程序修改
//    */
//      /* add your code here ^_^. */
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Debug_USART_Config();
//  /* 初始化系统滴答定时器 */
  SysTick_Init(); 
  LCD_Init();
  mem_init(SRAMIN);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
  GUI_Init();
  GUI_SetBkColor(GUI_BLUE);
  GUI_SetColor(GUI_YELLOW);
  GUI_Clear();
  GUI_SetFont(&GUI_Font24_ASCII);
  GUI_DispStringAt("ChenYiFaer", 0, 0);
  while(1)
  {    
  }

}

/*********************************************END OF FILE**********************/

