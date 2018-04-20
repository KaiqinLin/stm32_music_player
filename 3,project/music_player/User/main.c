#include "stm32f4xx.h"
#include "./usart/bsp_usart.h"
#include "./sdio/bsp_sdio.h"
#include "./led/bsp_led.h"
#include "./wm8978/bsp_wm8978.h"
#include "./systick/bsp_systick.h"
#include "./timer/bsp_timer.h"
#include "./key/bsp_key.h" 
//#include "./test/mp3Player.h"
#include "./lcd/bsp_lcd.h"
#include "./scheduler/scheduler.h"
#include "./malloc/malloc.h"
#include "common.h"
#include "ff.h"
//#include "./test/sdio_test.h"
#include "./test/lcd_test.h"
#include "GUI.h"
#include "GUIDemo.h"
#include "./player/player.h"
#include "./key/key_input.h"
#include "./ui/ui.h"

/**************** Private marco    *******************/


/**************** Global varialbles ******************/
FATFS fs;

void test_task(task_t *s, void *ctx)
{
  static uint32_t i = 0;
  GUI_GotoXY(10, 60);
  GUI_DispDec(i++, 4);
}
/**************** Task array        ******************/
task_t task_array[] = 
{
   {player_task,       (uint8_t *)"player_task", 100, 3, 0, &g_play_ctx},
   {test_task,         (uint8_t *)"test_task",    10, 2, 0, NULL},

   {key_input_task,    (uint8_t *)"key_task",      5, 1, 0, &g_key_input_ctx},
   {gui_task,          (uint8_t *)"gui_task",     50, 3, 0, &g_ui_ctx}
};


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
//  TIMx_Configuration(&GUI_TOUCH_Exec);
  /* Initialize th xpt2046 and ili9341 as also as the periph */
//  LCD_Init();
  /* Enable the CRC periph to support the GUI */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

  WM_SetCreateFlags(WM_CF_MEMDEV);
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

  /* scheduler timer config */
  TIM6_Configuration();
}


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  bsp_init();

  default_init(&g_sched, task_array, ARRAY_LEN(task_array));

  player_init(NULL, &g_play_ctx);
  sys_gui_init(NULL, &g_ui_ctx);
//  play_ctx.file_name = "0:/谭咏麟 - 一生中最爱.mp3";
//  GUIDEMO_Main();
  while(1)
  {
//    lcd_test_case();
//    mp3PlayerDemo("0:/谭咏麟 - 一生中最爱.mp3");
//    gui_touch_test_case();
    main_loop(&g_sched);
//    player_task(NULL, &play_ctx);
  }

}

/*********************************************END OF FILE**********************/

