#include "./lcd/bsp_lcd.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./systick/bsp_Systick.h"
#include "./lcd/bsp_xpt2046_lcd.h"



/**
  * @brief  LCD 初始化函数
  * @param  无
  * @retval 无
  */
void LCD_Init ( void )
{
  ILI9341_Init ();
  Delay_ms(100);
  
#if macLCD_XPT2046_ENABLE
  
    XPT2046_Init ();
#else

#endif
  
}


