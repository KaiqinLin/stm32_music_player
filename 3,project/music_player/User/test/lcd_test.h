#ifndef __LCD_TEST_H
#define __LCD_TEST_H
#include "stm32f4xx.h"

void Palette_Init ( void );

void Palette_draw_point ( uint16_t x, uint16_t y );

void lcd_test_case(void);

#endif /* __LCD_TEST_H*/

