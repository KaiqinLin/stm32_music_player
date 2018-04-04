#include "./lcd/bsp_lcd.h"
#include "./lcd/bsp_ili9341_lcd.h"



/**
  * @brief  LCD 初始化函数
  * @param  无
  * @retval 无
  */
void LCD_Init ( void )
{
	ILI9341_Init ();
  ILI9341_Clear(0, 0, 320, 240, macWHITE);
	
	#if macLCD_XPT2046_ENABLE
	
	  XPT2046_Init ();
	
	#endif
	
}


