#ifndef __KEY_INPUT_H
#define __KEY_INPUT_H
#include "stm32f4xx.h"
#include "./scheduler/scheduler.h"

void key_input_task(task_t *s, void *ctx);

#endif /*  __KEY_INPUT_H */
