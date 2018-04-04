#include "./led/bsp_led.h"

void LED_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(LED0_GPIO_Clk |
                         LED1_GPIO_Clk, ENABLE);

  GPIO_InitStructure.GPIO_Pin = LED0_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(LED0_GPIO_Port, &GPIO_InitStructure);

  
  GPIO_InitStructure.GPIO_Pin = LED1_Pin;
  GPIO_Init(LED1_GPIO_Port, &GPIO_InitStructure);

  GPIO_SetBits(LED0_GPIO_Port, LED0_Pin);
  GPIO_SetBits(LED1_GPIO_Port, LED1_Pin);
}


