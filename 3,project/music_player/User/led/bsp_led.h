#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "stm32f4xx.h"


#define LED0_Pin                  GPIO_Pin_8
#define LED0_GPIO_Port            GPIOB
#define LED0_GPIO_Clk             RCC_AHB1Periph_GPIOB

#define LED1_Pin                  GPIO_Pin_9
#define LED1_GPIO_Port            GPIOB
#define LED1_GPIO_Clk             RCC_AHB1Periph_GPIOB


void LED_GPIO_Config(void);

#endif /* __BSP_LED_H*/

