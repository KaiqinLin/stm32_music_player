#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "stm32f4xx.h"
#include "./scheduler/scheduler.h"

#define GENERAL_TIM               TIM2
#define GENERAL_TIM_CLK           RCC_APB1Periph_TIM2

#define GENERAL_TIM_IRQn          TIM2_IRQn
#define GENERAL_TIM_IRQHandler    TIM2_IRQHandler

#define SCHE_TIM                  TIM6
#define SCHE_TIM_CLK              RCC_APB1Periph_TIM6
#define SCHE_TIM_IRQn             TIM6_DAC_IRQn
#define SCHE_TIM_IRQHandler       TIM6_DAC_IRQHandler


void TIMx_Configuration(void(*cb)(void));

void TIM6_Configuration(void);
/* Get micros and millis function */
uint32_t micros(void);
uint32_t millis(void);

#endif /* __BSP_TIMER_H */

