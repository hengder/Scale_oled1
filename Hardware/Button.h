#ifndef _BUTTON_H
#define _BUTTON_H

#include "main.h"
#include "stdint.h"

// 原理图按下为低电平
#define RESET_BUTTON_Pin       GPIO_PIN_6
#define RESET_BUTTON_GPIO_Port GPIOA

// 对外提供的按键触发标志位
extern uint8_t Button_Event_Flag;

#endif
