#include "stm32f4xx.h"
#include "./usart/bsp_usart.h"
#include "./sdio/bsp_sdio.h"
#include "./led/bsp_led.h"
#include "./wm8978/bsp_wm8978.h"
#include "./systick/bsp_systick.h"
#include "./key/bsp_key.h" 
#include "./test/mp3Player.h"
#include "./lcd/bsp_lcd.h"
#include "common.h"
#include "ff.h"
#include "./test/sdio_test.h"
#include "./test/lcd_test.h"
//#include "./test/Recorder.h"
#include "GUI.h"

/**************** Private marco    *******************/


/**************** Global varialbles ******************/
FATFS fs;





/**
 * @brief :Initialize the bsp periph.
 * @param :None
 */

void bsp_init(void)
{
  FRESULT result;                                   /*<! The return of mounting the sd card >*/

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  LED_GPIO_Config();
  Key_GPIO_Config();
  SysTick_Init();
  /* Initialize th xpt2046 and ili9341 as also as the periph */
  LCD_Init();
  /* Enable the CRC periph to support the GUI */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
  GUI_Init();
  Debug_USART_Config();

  result = f_mount(&fs, "0:", 1);
  if (result != FR_OK)
  {
    debug("%s :SD card mouted failed.\r\n",
          __func__);
    while(1);
  }

  if (wm8978_Init() == 0 ) {
    debug("%s :Initialize the wm8978 failed.\r\n",
          __func__);
    while(1);
  }

}


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  bsp_init();

  while(1)
  {
    
//    mp3PlayerDemo("0:/谭咏麟 - 一生中最爱.mp3");
    RecorderDemo();
  }

}

/*********************************************END OF FILE**********************/

