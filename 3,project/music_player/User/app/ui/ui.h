#ifndef __UI_H
#define __UI_H
#include "stm32f4xx.h"

typedef struct _ui_ctx_t {
  uint8_t  wim;
} ui_ctx_t;

extern ui_ctx_t   ui_ctx;

void ui_task(void);

#endif /* __UI_H*/

