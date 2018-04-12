#include "./test/lcd_test.h"
#include "./lcd/bsp_xpt2046_lcd.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./lcd/bsp_lcd.h"
#include "GUI.h"
#include "./systick/bsp_systick.h"



 /**
  * @brief  初始化画板界面
  * @param  无
  * @retval 无
  */
void Palette_Init ( void )
{
  while( ! XPT2046_Touch_Calibrate () );      //等待触摸屏校正完毕
  
  
  ILI9341_GramScan ( 5 );

  /* 整屏清为白色 */
  ILI9341_Clear ( 0, 0, 320, 240, macWHITE );
  
  /* 画两条直线 */
  ILI9341_Clear ( 39,  0,  1, 30, macBLACK );
  ILI9341_Clear (  0, 29, 40,  1, macBLACK );

  ILI9341_Clear ( 0,  30, 40, 30, macGREEN );
  ILI9341_Clear ( 0,  60, 40, 30, macBLUE  );
  ILI9341_Clear ( 0,  90, 40, 30, macBRED  );
  ILI9341_Clear ( 0, 120, 40, 30, macGRED  );
  ILI9341_Clear ( 0, 150, 40, 30, macGBLUE );
  ILI9341_Clear ( 0, 180, 40, 30, macBLACK );
  ILI9341_Clear ( 0, 210, 40, 30, macRED   );  
  
  ILI9341_DispString_EN ( 7, 10, "CLR", macBACKGROUND, macRED );
  
  
}


 /**
  * @brief  在LCD指定位置描点
  * @param  x：X坐标
  * @param  y：Y坐标
  * @retval 无
  */  
void Palette_draw_point ( uint16_t x, uint16_t y )
{
  uint16_t y_pos = y;
  static uint16_t Pen_color = macBLACK;   //画笔默认为黑色 

  
  if ( x < 40 )    //在画板内取色 
  {
    if ( y > 30 )
      Pen_color = ( y_pos < 60  ) ? macGREEN :
                  ( y_pos < 90  ) ? macBLUE  :
                  ( y_pos < 120 ) ? macBRED  :
                  ( y_pos < 150 ) ? macGRED  :
                  ( y_pos < 180 ) ? macGBLUE :
                  ( y_pos < 210 ) ? macBLACK :
                  ( y_pos < 240 ) ? macRED   : macBLUE;
    
    else          //清屏
    {      
      #if 0
        ILI9341_Clear ( 40, 0, 280, 240, macBLACK );
      #else
        ILI9341_Clear ( 40, 0, 280, 240, macWHITE );
      #endif
      
      return;
      
    }
    
  }
  
  else
  {
    #if 0
    ILI9341_SetPointPixel (   x,   y, Pen_color );
    ILI9341_SetPointPixel ( x+1,   y, Pen_color );
    ILI9341_SetPointPixel (   x, y+1, Pen_color );
    ILI9341_SetPointPixel ( x+1, y+1, Pen_color );
    
    #else
    ILI9341_SetPointPixel (   x,   y, Pen_color );
    ILI9341_SetPointPixel ( x-1,   y, Pen_color );
    ILI9341_SetPointPixel (   x, y-1, Pen_color );
    ILI9341_SetPointPixel ( x+1,   y, Pen_color );
    ILI9341_SetPointPixel (   x, y+1, Pen_color );
    
    #endif
    
  }  
  
}

void lcd_test_case(void)
{

  
  Palette_Init ();      //触摸取色板初始化
  

  while ( 1 )
  {
    strType_XPT2046_Coordinate strDisplayCoordinate;
    
    
    if ( ucXPT2046_TouchFlag == 1 )                                                         //如果触摸了屏幕
    {
      if ( XPT2046_Get_TouchedPoint ( & strDisplayCoordinate, & strXPT2046_TouchPara ) )   //获取触摸点的坐标  
        Palette_draw_point ( strDisplayCoordinate .x, strDisplayCoordinate .y );                       

    }    
    
  }
}
void gui_touch_test_case(void)
{
   GUI_PID_STATE TOUCH_STAT;
   int xPhys, yPhys;

   GUI_SetFont(&GUI_Font20_ASCII);
   GUI_CURSOR_Show();
   GUI_CURSOR_Select(&GUI_CursorCrossL);


   GUI_SetBkColor(GUI_WHITE);
   GUI_Clear();

   while (1) {
     GUI_TOUCH_GetState(&TOUCH_STAT);
     xPhys = GUI_TOUCH_GetxPhys();
     yPhys = GUI_TOUCH_GetyPhys();

     GUI_SetColor(GUI_BLUE);
     GUI_DispStringAt("Anolog input:\n", 0, 40);
     GUI_GotoY(GUI_GetDispPosY() + 2);
     GUI_DispString("x:");
     GUI_DispDec(xPhys, 4);
     GUI_DispString(",y:");
     GUI_DispDec(yPhys, 4);

     GUI_SetColor(GUI_RED);
     GUI_GotoY(GUI_GetDispPosY() + 4);
     GUI_DispString("x:");
     GUI_DispDec(TOUCH_STAT.x, 4);
     GUI_DispString(",y:");
     GUI_DispDec(TOUCH_STAT.y, 4);
     Delay_10us(5000);
   }
}



