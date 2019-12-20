#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "stm32f4xx.h"


#define LED0_Pin                  GPIO_Pin_8
#define LED0_GPIO_Port            GPIOB
#define LED0_GPIO_Clk             RCC_AHB1Periph_GPIOB

#define LED1_Pin                  GPIO_Pin_9
#define LED1_GPIO_Port            GPIOB
#define LED1_GPIO_Clk             RCC_AHB1Periph_GPIOB

#define BAT_CHRG_N_Pin            GPIO_Pin_2
#define BAT_CHRG_N_Port           GPIOC
#define BAT_CHRG_GPIO_Clk         RCC_AHB1Periph_GPIOC

#define BAT_STDBY_N_Pin            GPIO_Pin_3
#define BAT_STDBY_N_Port           GPIOC
#define BAT_STDBY_GPIO_Clk         RCC_AHB1Periph_GPIOC

#define   CHRG_READ              GPIO_ReadInputDataBit(BAT_CHRG_N_Port,  BAT_CHRG_N_Pin)  ==  Bit_RESET
#define   STDBY_READ             GPIO_ReadInputDataBit(BAT_STDBY_N_Port, BAT_STDBY_N_Pin) ==  Bit_RESET

void LED_GPIO_Config(void);

#endif /* __BSP_LED_H*/

