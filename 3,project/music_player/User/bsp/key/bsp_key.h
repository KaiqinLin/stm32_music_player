#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx.h"

//引脚定义
/*******************************************************/
#define KEY2_PIN                  GPIO_Pin_0
#define KEY2_GPIO_PORT            GPIOC
#define KEY2_GPIO_CLK             RCC_AHB1Periph_GPIOC

#define KEY3_PIN                  GPIO_Pin_1
#define KEY3_GPIO_PORT            GPIOC
#define KEY3_GPIO_CLK             RCC_AHB1Periph_GPIOC

#define KEY4_PIN                  GPIO_Pin_0
#define KEY4_GPIO_PORT            GPIOE
#define KEY4_GPIO_CLK             RCC_AHB1Periph_GPIOE

#define KEY5_PIN                  GPIO_Pin_1
#define KEY5_GPIO_PORT            GPIOE
#define KEY5_GPIO_CLK             RCC_AHB1Periph_GPIOE

#define KEY6_PIN                  GPIO_Pin_2
#define KEY6_GPIO_PORT            GPIOE
#define KEY6_GPIO_CLK             RCC_AHB1Periph_GPIOE

#define KEY7_PIN                  GPIO_Pin_3
#define KEY7_GPIO_PORT            GPIOE
#define KEY7_GPIO_CLK             RCC_AHB1Periph_GPIOE

#define KEY8_PIN                  GPIO_Pin_4
#define KEY8_GPIO_PORT            GPIOE
#define KEY8_GPIO_CLK             RCC_AHB1Periph_GPIOE

#define KEY9_PIN                  GPIO_Pin_5
#define KEY9_GPIO_PORT            GPIOE
#define KEY9_GPIO_CLK             RCC_AHB1Periph_GPIOE

#define KEY10_PIN                  GPIO_Pin_6
#define KEY10_GPIO_PORT            GPIOE
#define KEY10_GPIO_CLK             RCC_AHB1Periph_GPIOE
/*******************************************************/

 /** 按键按下标置宏
 * 按键按下为高电平，设置 KEY_ON=1， KEY_OFF=0
 * 若按键按下为低电平，把宏设置成KEY_ON=0 ，KEY_OFF=1 即可
 */
#define KEY_ON 1
#define KEY_OFF 0

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);

#endif /* __LED_H */

