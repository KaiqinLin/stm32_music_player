#ifndef __BSP_RTC_H
#define __BSP_RTC_H
#include "stm32f4xx.h"
#include "common.h"
#include "./systick/bsp_systick.h"

// 异步分频因子
#define ASYNCHPREDIV         0X7F
// 同步分频因子
#define SYNCHPREDIV          0XFF

uint8_t RTC_Config(void);

#endif /* __BSP_RTC_H */
