#include "./lcd/bsp_lcd.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./systick/bsp_Systick.h"



/**
  * @brief  LCD 初始化函数
  * @param  无
  * @retval 无
  */
void LCD_Init ( void )
{
	ILI9341_Init ();
  Delay_10us(1000);
  ILI9341_Clear(0, 0, 320, 240, macBLACK);
	
	#if macLCD_XPT2046_ENABLE
	
	  XPT2046_Init ();
	
	#endif
	
}


