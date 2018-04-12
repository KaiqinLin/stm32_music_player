#include "GUI_X_Touch_Analog.h"
#include "./lcd/bsp_xpt2046_lcd.h"

void GUI_TOUCH_X_ActivateX(void) {
//    XPT2046_WriteCMD(macXPT2046_CHANNEL_X);
}

void GUI_TOUCH_X_ActivateY(void) {
//    XPT2046WriteCMD(macXPT2046_CHANNEL_Y);
}

int GUI_TOUCH_X_MeasureX(void) {

    return XPT2046_ReadADC_Smooth_XOY(macXPT2046_CHANNEL_X);
}
int GUI_TOUCH_X_MeasureY(void) {
    return XPT2046_ReadADC_Smooth_XOY(macXPT2046_CHANNEL_Y);
}


