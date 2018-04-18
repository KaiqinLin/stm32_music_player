#ifndef __KEY_INPUT_H
#define __KEY_INPUT_H
#include "stm32f4xx.h"
#include "./scheduler/scheduler.h"
#include "./key/bsp_key.h"



#define   KEY_VOLUME_UP_READ           GPIO_ReadInputDataBit(KEY2_GPIO_PORT,    KEY2_PIN)  ==  Bit_SET
#define   KEY_VOLUME_DOWN_READ         GPIO_ReadInputDataBit(KEY3_GPIO_PORT,    KEY3_PIN)  ==  Bit_SET
#define   KEY_BACK_READ                GPIO_ReadInputDataBit(KEY4_GPIO_PORT,    KEY4_PIN)  ==  Bit_SET
#define   KEY_MENU_READ                GPIO_ReadInputDataBit(KEY5_GPIO_PORT,    KEY5_PIN)  ==  Bit_SET
#define   KEY_RIGHT_READ               GPIO_ReadInputDataBit(KEY6_GPIO_PORT,    KEY6_PIN)  ==  Bit_SET
#define   KEY_DOWN_READ                GPIO_ReadInputDataBit(KEY7_GPIO_PORT,    KEY7_PIN)  ==  Bit_SET
#define   KEY_MID_READ                 GPIO_ReadInputDataBit(KEY8_GPIO_PORT,    KEY8_PIN)  ==  Bit_SET
#define   KEY_UP_READ                  GPIO_ReadInputDataBit(KEY9_GPIO_PORT,    KEY9_PIN)  ==  Bit_SET
#define   KEY_LEFT_READ                GPIO_ReadInputDataBit(KEY10_GPIO_PORT,   KEY10_PIN) ==  Bit_SET

typedef struct _key_input_ctx_t {
  uint8_t  vol_up_falg;
  uint8_t  vol_down_flag;
  uint8_t  back_flag;
  uint8_t  menu_flag;
  uint8_t  right_flag;
  uint8_t  down_flag;
  uint8_t  mid_flag;
  uint8_t  up_flag;
  uint8_t  left_flag;
} key_input_ctx_t;



void key_input_task(task_t *s, void *ctx);

#endif /*  __KEY_INPUT_H */
